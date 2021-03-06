// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "bs_server.h"

#include <cmath>
#include <functional>

#include "bot.h"
#include "bot_config.h"

#include "./make_unique.h"
#include "./messages/update_msg.h"

namespace botscript_server {

bot_load_lock::bot_load_lock(bs_server& server, std::string identifier)
    : server_(server), identifier_(std::move(identifier)) {
  server.bot_creation_blocklist_.insert(identifier_);
}

bot_load_lock::~bot_load_lock() {
  server_.bot_creation_blocklist_.erase(identifier_);
}

bs_server::bs_server(bs_server_options options,
                     boost::asio::io_service* io_service,
                     std::shared_ptr<dust::key_value_store> store,
                     sid_callback activity_cb,
                     session_end_cb session_end_callback)
    : options_(std::move(options)),
      io_service_(io_service),
      signals_(*io_service_),
      store_(std::move(store)),
      users_(store_, "users"),
      activity_cb_(std::move(activity_cb)),
      session_end_cb_(std::move(session_end_callback)) {
  update_packages();
  for (const auto& user_doc : users_.children()) {
    update_session(user(user_doc));
  }
  load_bots();

#if not defined(_WIN32) && not defined(_WIN64)
  signals_.add(SIGUSR1);
  listen_for_update_packages_signal();
#endif
}

void bs_server::listen_for_update_packages_signal() {
#if not defined(_WIN32) && not defined(_WIN64)
  signals_.async_wait([this](boost::system::error_code /*ec*/, int /*signo*/) {
    update_packages();
    listen_for_update_packages_signal();
  });
#endif
}

void bs_server::update_packages() {
  std::cout << "Loading packages... ";
  botscript::bot::load_packages(options_.packages_path);
  packages_.resize(0);
  for (const auto& package : botscript::bot::packages_) {
    std::cout << package.second->name() << " ";
    packages_.push_back(package.second->interface_desc());
  }
  std::cout << std::endl;
}

update_cb bs_server::sid_cb(const std::string& sid) {
  return [this, sid](std::string id, std::string k, std::string v) {
    if (options_.botlog && k == "log") {
      std::cout << v;
    }

    std::vector<msg_ptr> out;
    out.emplace_back(make_unique<update_msg>(id, k, v));
    activity_cb_(sid, std::move(out));
  };
}

update_cb bs_server::print_cb() {
  return [this](std::string /* id */, std::string k, std::string v) {
    if (options_.botlog && k == "log") {
      std::cout << v;
    }
  };
}

void bs_server::handle_connection_close(const std::string& sid) {
  // Resolve the session ID to a user.
  typedef session_set::index<index_session_id>::type session_id_index;
  auto& sessions_indexed_by_sid = sessions_.get<index_session_id>();
  session_id_index::iterator it = sessions_indexed_by_sid.find(sid);
  if (it == sessions_indexed_by_sid.end()) {
    return;
  }

  // Set the update callback of the user's bots to print.
  user u = it->u;
  auto print_upd_cb = print_cb();
  for (const auto& config : u.bot_configs()) {
    auto it = bots_.find(config->identifier());
    if (it != bots_.end()) {
      it->second->update_callback_ = print_upd_cb;
    }
  }

  // Trigger session end callback.
  io_service_->post([this, sid]() { session_end_cb_(sid); });
}

void bs_server::load_bot(
    bot_load_info load_info,
    std::shared_ptr<std::vector<bot_load_info>> load_infos) {
  using std::placeholders::_1;
  using std::placeholders::_2;

  auto config = load_info.config;

  if (options_.forceproxy && config->value_of("base_proxy").empty()) {
    std::cout << "[ERROR] Config without proxy: " << config->identifier()
              << std::endl;
    load_further_bot(load_infos);
    config->inactive(true);
    return;
  }

  std::cout << "Loading bot " << config->identifier() << std::endl;
  auto load_cb = std::bind(&bs_server::on_bot_load, this, _1, _2, load_infos,
                           load_info.load_lock);
  auto bot = std::make_shared<botscript::bot>(io_service_);
  bot->update_callback_ = print_cb();
  bot->init(config, load_cb);
}

void bs_server::load_bots() {
  // Create configurations vector.
  auto load_infos = std::make_shared<std::vector<bot_load_info>>();
  for (const auto& user_doc : users_.children()) {
    auto user_bots = user(user_doc).bot_configs();

    std::cerr << "Adding bots from user " << user(user_doc).username() << ":\n";
    std::for_each(
        user_bots.begin(), user_bots.end(),
        [&load_infos, this](const bot_config_ptr& conf) {
          std::cout << "\t" << conf->identifier() << "\n";

          load_infos->push_back({conf, std::make_shared<bot_load_lock>(
                                           *this, conf->identifier())});
        });
  }

  // Load the first N bots.
  int i = 0;
  while (i++ < 50) {
    load_further_bot(load_infos);
  }
}

void bs_server::load_further_bot(
    std::shared_ptr<std::vector<bot_load_info>> load_infos) {
  if (!load_infos->empty()) {
    bot_load_info load_info = *load_infos->rbegin();
    load_infos->resize(load_infos->size() - 1);
    load_bot(load_info, load_infos);
  }
}

void bs_server::on_bot_load(
    std::shared_ptr<botscript::bot> bot, std::string err,
    std::shared_ptr<std::vector<bot_load_info>> load_infos,
    std::shared_ptr<bot_load_lock>) {
  bool success = err.empty();
  bot->config()->inactive(!success);

  if (success) {
    bots_[bot->config()->identifier()] = bot;
    std::cout << "[INFO ] Successfully loaded " << bot->config()->identifier()
              << std::endl;
  } else {
    std::cout << "[ERROR] Could not load " << bot->config()->identifier()
              << ": " << err << std::endl;
    bot->shutdown();
  }

  load_further_bot(load_infos);
}

void bs_server::stop() {
  for (auto& bot : bots_) {
    bot.second->shutdown();
  }
  bots_.clear();
}

void bs_server::update_session(const user& u) {
  if (!u.session_active()) {
    return;
  }

  // Update sessions_ multi-set.
  typedef session_set::index<index_user>::type user_index;
  auto& users_indexed_by_name = sessions_.get<index_user>();
  user_index::iterator it = users_indexed_by_name.find(u);
  if (it == users_indexed_by_name.end()) {
    sessions_.insert({u.session_id(), u});
  } else {
    session s = *it;
    s.id = u.session_id();
    users_indexed_by_name.replace(it, s);
  }

  // Set callback that informs the user about updates.
  for (const auto& config : u.bot_configs()) {
    auto it = bots_.find(config->identifier());
    if (it != bots_.end()) {
      it->second->update_callback_ = sid_cb(u.session_id());
    }
  }
}

std::map<std::string, std::string> bs_server::bot_logs(const user& u) const {
  std::map<std::string, std::string> id_log_map;
  for (const auto& config : u.bot_configs()) {
    const auto id_match = bots_.find(config->identifier());
    if (id_match != bots_.cend()) {
      id_log_map[config->identifier()] = id_match->second->log_msgs();
    }
  }
  return id_log_map;
}

}  // namespace botscript_server
