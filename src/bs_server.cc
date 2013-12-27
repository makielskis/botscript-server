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

bs_server::bs_server(boost::asio::io_service* io_service,
                     std::shared_ptr<dust::key_value_store> store,
                     std::vector<std::string> packages,
                     sid_callback activity_cb,
                     session_end_cb session_end_callback)
    : io_service_(io_service),
      store_(std::move(store)),
      users_(store_, "users"),
      packages_(std::move(packages)),
      activity_cb_(std::move(activity_cb)),
      session_end_cb_(std::move(session_end_callback)) {
  for (const auto& user_doc : users_.children()) {
    update_session(user(user_doc));
  }
  load_bots();
}

update_cb bs_server::sid_cb(const std::string& sid) {
  return [this, sid](std::string id, std::string k, std::string v) {
    if (k == "log") {
      std::cout << v;
    }

    std::vector<msg_ptr> out;
    out.emplace_back(make_unique<update_msg>(id, k, v));
    activity_cb_(sid, std::move(out));
  };
}

update_cb bs_server::print_cb() {
  return [](std::string /* id */, std::string k, std::string v) {
    if (k == "log") {
      std::cout << v;
    }
  };
}

void bs_server::handle_connection_close(const std::string& sid) {
  session_end_cb_(sid);

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
}

void bs_server::load_bot(std::shared_ptr<std::vector<bot_config_ptr>> configs,
                         std::size_t index) {
  using std::placeholders::_1;
  using std::placeholders::_2;
  std::cout << "Loading bot " << configs->at(index)->identifier() << std::endl;
  auto load_cb = std::bind(&bs_server::on_bot_load, this, _1, _2, configs);
  auto bot = std::make_shared<botscript::bot>(io_service_);
  bot->update_callback_ = print_cb();
  bot->init(configs->at(index), load_cb);
  configs->erase(configs->begin() + index);
}

void bs_server::load_bots() {
  // Create configurations vector.
  auto configs = std::make_shared<std::vector<bot_config_ptr>>();
  for (const auto& user_doc : users_.children()) {
    auto user_bots = user(user_doc).bot_configs();
    configs->insert(configs->end(), user_bots.begin(), user_bots.end());

    std::cerr << "Adding bots from user " << user(user_doc).username() << ":\n";
    std::for_each(user_bots.begin(), user_bots.end(),
                  [](const std::shared_ptr<botscript::bot_config>& conf) {
                    std::cout << "\t" << conf->identifier() << "\n";
                  });
  }

  // Load the first N bots.
  int start = configs->size() - 1;
  int end = std::max(start - 50, 0);
  for (int i = start; i >= end; --i) {
    load_bot(configs, i);
  }
}

void bs_server::on_bot_load(
    std::shared_ptr<botscript::bot> bot,
    std::string err,
    std::shared_ptr<std::vector<bot_config_ptr>> configs) {
  bool success = err.empty();
  bot->configuration().inactive(!success);

  if (success) {
    bots_[bot->configuration().identifier()] = bot;
    std::cout << "[INFO ] Successfully loaded "
              << bot->configuration().identifier() << std::endl;
  } else {
    std::cout << "[ERROR] Could not load " << bot->configuration().identifier()
              << ": " << err << std::endl;
    bot->shutdown();
  }

  if (!configs->empty()) {
    load_bot(configs, configs->size() - 1);
  }
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
    sessions_.insert( { u.session_id(), u });
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
