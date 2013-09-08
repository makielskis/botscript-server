// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./bot_manager.h"

#include <iostream>
#include <utility>
#include <stdexcept>

#include "bot.h"

#include "./messages/outgoing_msgs.h"

#define LOAD_SIZE (10)

using boost::system::error_code;
namespace ph = std::placeholders;
namespace bs = botscript;

namespace botscript_server {

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

bot_manager::bot_manager(const std::string& packages_path,
                         config_store& config_store,
                         user_store& user_store,
                         sid_callback activity_cb,
                         std::function<void (std::string)> session_end_cb,
                         boost::asio::io_service* io_service)
    : config_store_(config_store),
      user_store_(user_store),
      io_service_(io_service),
      packages_(std::move(bs::bot::load_packages(packages_path))),
      sid_callback_(std::move(activity_cb)),
      session_end_cb_(std::move(session_end_cb)) {
}

void bot_manager::load_bots(std::function<void ()> on_finish) {
  std::vector<std::string> configs = config_store_.get_all();
  auto configs_ptr = std::make_shared<std::vector<std::string>>(configs.begin(), configs.end());
  auto load_cb = std::bind(&bot_manager::on_bot_load, this, ph::_1, ph::_2, configs_ptr);

  int count = 0;
  while (count < 10 && configs_ptr->size() > 0) {
    std::string config = *configs_ptr->rbegin();
    configs_ptr->resize(configs_ptr->size() - 1);
    std::cout << "Loading bot configuration: " << config << "\n";
    auto bot = std::make_shared<bs::bot>(io_service_);
    bot->callback_ = std::bind(&bot_manager::print_cb, this, bot, ph::_1, ph::_2, ph::_3);
    bot->init(config, load_cb);
  }
}

void bot_manager::on_bot_load(
    std::shared_ptr<bs::bot> bot,
    std::string err,
    std::shared_ptr<std::vector<std::string>> configs_ptr) {
  if (err.empty()) {
    std::cout << "Loaded " << bot->identifier() << "\n";
    bots_[bot->identifier()] = bot;
  } else {
    std::cout << "Unable to load " << bot->identifier() << " [" << err << "]\n";
    bot->shutdown();
  }

  if (configs_ptr->size() != 0) {
    std::string config = *configs_ptr->rbegin();
    configs_ptr->resize(configs_ptr->size() - 1);
    std::cout << "Loading bot configuration: " << config << "\n";
    auto load_cb = std::bind(&bot_manager::on_bot_load, this, ph::_1, ph::_2, configs_ptr);
    auto bot = std::make_shared<bs::bot>(io_service_);
    bot->callback_ = std::bind(&bot_manager::print_cb, this, bot, ph::_1, ph::_2, ph::_3);
    bot->init(config, load_cb);
  }
}

void bot_manager::handle_connection_close(const std::string& sid) {
  const auto it = sid_bot_ids_map_.find(sid);
  if (it != sid_bot_ids_map_.end()) {
    for (const auto& id : it->second) {
      auto& bot = bots_[id];
      bot->callback_ = std::bind(&bot_manager::print_cb, this, bot, ph::_1, ph::_2, ph::_3);
    }
  }
  sid_bot_ids_map_.erase(it);
  session_end_cb_(sid);
}

void bot_manager::handle_login_msg(
    login_msg m,
    sid_callback cb) {
  return user_store_.login(m.username(), m.password(), [=](std::string sid, error_code e) {
    if (e) {
      // Failure: responde with failure message.
      std::vector<outgoing_msg_ptr> out;
      out.emplace_back(make_unique<failure_msg>(0, m.type(), e.value(), e.message()));
      return cb("", std::move(out));
    }

    return on_login(sid, 0, m.type(), cb);
  });
}

void bot_manager::handle_register_msg(
    register_msg m,
    sid_callback cb) {
  return user_store_.add_user(m.username(), m.password(), m.email(), [=](std::string sid, error_code e) {
    std::vector<outgoing_msg_ptr> out;

    if (e) {
      // Failure: responde with failure message.
      out.emplace_back(make_unique<failure_msg>(0, m.type(), e.value(), e.message()));
      return cb("", std::move(out));
    }

    // Success: send session ID, packages, bots, and bot logs.
    out.emplace_back(make_unique<session_msg>(0, sid));
    out.emplace_back(make_unique<account_msg>(m.email()));
    out.emplace_back(make_unique<packages_msg>(packages_));
    out.emplace_back(make_unique<bots_msg>(std::map<std::string, std::string>()));
    return cb(sid, std::move(out));
  });
}

void bot_manager::handle_user_msg(
    user_msg m,
    sid_callback cb) {
  return on_login(m.sid(), 0, m.type(), cb);
}

void bot_manager::handle_create_bot_msg(
    create_bot_msg m,
    msg_callback cb) {
  // Load configuration.
  bs::config c;
  try {
    c = bs::config(m.config());
  } catch (const std::runtime_error& e) {
    std::vector<outgoing_msg_ptr> out;
    out.emplace_back(make_unique<failure_msg>(0, m.type(), 8, e.what()));
    return cb(std::move(out));
  }

  // Create identifier.
  std::string id = bs::bot::identifier(c.username(), c.package(), c.server());

  // Add bot to user store.
  return user_store_.add_bot(m.sid(), id, [=](error_code e) {
    std::vector<outgoing_msg_ptr> out;

    if (e) {
      // Failure: responde with failure message.
      out.emplace_back(make_unique<failure_msg>(0, m.type(), e.value(), e.message()));
      return cb(std::move(out));
    }

    // Create bot object.
    auto b = std::make_shared<bs::bot>(io_service_);

    // Send updates out to the activity callback.
    b->callback_ = std::bind(&bot_manager::sid_cb, this, m.sid(), b, ph::_1, ph::_2, ph::_3);

    // Check blocklist.
    if (bot_creation_blocklist_.find(id) != bot_creation_blocklist_.end()) {
      std::vector<outgoing_msg_ptr> out;
      out.emplace_back(make_unique<failure_msg>(0, m.type(), 10, "Bot already exists"));
      return cb(std::move(out));
    }
    bot_creation_blocklist_.insert(id);

    // Load configuration.
    return b->init(m.config(), [=](std::shared_ptr<bs::bot> created_bot, std::string err) {
      // Remove from botlist (independent from init result).
      bot_creation_blocklist_.erase(id);

      std::vector<outgoing_msg_ptr> out;

      if (!err.empty()) {
        // Add bot to user store.
        return user_store_.remove_bot(m.sid(), id, [=](error_code e) {
          // Can't react to error here, need to inform user about creation error.
          // Bot creation failed (i.e. bad proxy): responde with failure message.
          std::vector<outgoing_msg_ptr> out;
          out.emplace_back(make_unique<failure_msg>(0, m.type(), 11, err));
          return cb(std::move(out));
        });
      }


      // Store bot.
      bots_[id] = created_bot;

      // Associate bot identifier with this session.
      sid_bot_ids_map_[m.sid()].insert(id);

      // Add bot config to config store.
      config_store_.add(created_bot, [=](error_code e) {
        if (e) {
          // This should not happen!
          std::vector<outgoing_msg_ptr> out;
          out.emplace_back(make_unique<failure_msg>(0, m.type(), e.value(), e.message()));
          return cb(std::move(out));
        }

        // Send new bot list to the user ( = success indicator)
        user_store_.get_bots(m.sid(), [=](std::vector<std::string> bots, error_code e) {
          std::vector<outgoing_msg_ptr> out;

          if (e) {
            // This should not happen (inconsistent database)!
            out.emplace_back(make_unique<failure_msg>(0, m.type(), e.value(), e.message()));
            return cb(std::move(out));
          }

          out.emplace_back(make_unique<bots_msg>(get_bot_configs(bots)));
          return cb(std::move(out));
        });
      });
    });
  });
}

void bot_manager::handle_delete_bot_msg(
    delete_bot_msg m,
    msg_callback cb) {
  return user_store_.remove_bot(m.sid(), m.identifier(), [=](error_code e) {
    if (e) {
      // Failure: respond with failure message.
      std::vector<outgoing_msg_ptr> out;
      out.emplace_back(make_unique<failure_msg>(0, m.type(), e.value(), e.message()));
      return cb(std::move(out));
    }

    std::vector<outgoing_msg_ptr> out;

    // Search for bot in bot list.
    auto it = bots_.find(m.identifier());
    if (it == bots_.end()) {
      out.emplace_back(make_unique<failure_msg>(0, m.type(), 12, "fatal: bot not found"));
      return cb(std::move(out));
    }

    // Remove bot from bot map.
    it->second->shutdown();
    bots_.erase(it);

    // Remove bot from sid -> bot ids list.
    const auto sid_ids_it = sid_bot_ids_map_.find(m.sid());
    if (sid_ids_it != sid_bot_ids_map_.end()) {
      auto& bots = sid_ids_it->second;
      const auto bot_id_it = bots.find(m.identifier());
      if (bot_id_it != bots.cend()) {
        bots.erase(bot_id_it);
      }
    }

    // Send new bot list to the user ( = success message).
    return user_store_.get_bots(m.sid(), [=](std::vector<std::string> bots, error_code e) {
      std::vector<outgoing_msg_ptr> out;

      if (e) {
        // This should not happen (inconsistent database)!
        out.emplace_back(make_unique<failure_msg>(0, m.type(), e.value(), e.message()));
        return cb(std::move(out));
      }

      out.emplace_back(make_unique<bots_msg>(get_bot_configs(bots)));
      return cb(std::move(out));
    });
  });
}

void bot_manager::handle_execute_bot_msg(
    execute_bot_msg m,
    msg_callback cb) {
  std::vector<outgoing_msg_ptr> out;

  const auto it = sid_bot_ids_map_.find(m.sid());
  if (it == sid_bot_ids_map_.cend()) {
    out.emplace_back(make_unique<failure_msg>(0, m.type(), 12, "fatal: bot not found"));
    return cb(std::move(out));
  }

  const auto bot_id_it = it->second.find(m.identifier());
  if (bot_id_it == it->second.end()) {
    out.emplace_back(make_unique<failure_msg>(0, m.type(), 12, "fatal: bot not found"));
    return cb(std::move(out));
  }

  bots_[m.identifier()]->execute(m.command(), m.argument());
  return cb(std::move(out));
}

void bot_manager::handle_reactivate_bot_msg(
    reactivate_bot_msg m,
    msg_callback cb) {
}

void bot_manager::handle_delete_update_msg(
    delete_update_msg m,
    msg_callback cb) {
  // First: get bots (getting them after the deletion is not possible)
  // -> required to delete all bots of the deleted user.
  return user_store_.get_bots(m.sid(), [=](std::vector<std::string> bots, error_code e) {
    std::vector<outgoing_msg_ptr> out;

    if (e) {
      // Failure: respond with failure message.
      out.emplace_back(make_unique<failure_msg>(0, m.type(), e.value(), e.message()));
      return cb(std::move(out));
    }

    // Remove user from store.
    return user_store_.remove_user(m.sid(), m.current_pw(), [=](error_code e) {
      std::vector<outgoing_msg_ptr> out;
      if (e) {
        // Failure: respond with failure message.
        out.emplace_back(make_unique<failure_msg>(0, m.type(), e.value(), e.message()));
      } else {
        // Success: cleanup bots and bot maps.
        const auto sid_bots_it = sid_bot_ids_map_.find(m.sid());
        if (sid_bots_it != sid_bot_ids_map_.end()) {
          for (const std::string& bot_id : sid_bots_it->second) {
            const auto it = bots_.find(bot_id);
            if (it != bots_.end()) {
              it->second->shutdown();
              bots_.erase(it);
            }
          }
          sid_bot_ids_map_.erase(sid_bots_it);
        }

        // Inform listener.
        session_end_cb_(m.sid());

        out.emplace_back(make_unique<success_msg>(0, m.type()));
      }
      return cb(std::move(out));
    });
  });
}

void bot_manager::handle_email_update_msg(
    email_update_msg m,
    msg_callback cb) {
  return user_store_.set_email(m.sid(), m.current_pw(), m.new_email(), [=](error_code e) {
    std::vector<outgoing_msg_ptr> out;

    if (e) {
      out.emplace_back(make_unique<failure_msg>(0, m.type(), e.value(), e.message()));
    } else {
      out.emplace_back(make_unique<success_msg>(0, m.type()));
    }

    return cb(std::move(out));
  });
}

void bot_manager::handle_password_update_msg(
    password_update_msg m,
    msg_callback cb) {
  return user_store_.set_password(m.sid(), m.current_pw(), m.new_pw(), [=](error_code e) {
    std::vector<outgoing_msg_ptr> out;

    if (e) {
      out.emplace_back(make_unique<failure_msg>(0, m.type(), e.value(), e.message()));
    } else {
      out.emplace_back(make_unique<success_msg>(0, m.type()));
    }

    return cb(std::move(out));
  });
}

void bot_manager::on_login(
    std::string sid,
    int message_id,
    std::vector<std::string> message_type,
    sid_callback cb) {
  return user_store_.get_email(sid, [=](std::string email, error_code e) {
    if (e) {
      // Failure: respond with failure message.
      std::vector<outgoing_msg_ptr> out;
      out.emplace_back(make_unique<failure_msg>(message_id, message_type, e.value(), e.message()));
      return cb("", std::move(out));
    }

    // Success: get bots that are owned by this user.
    return user_store_.get_bots(sid, [=](std::vector<std::string> bots, error_code e) {
      std::vector<outgoing_msg_ptr> out;

      if (e) {
        // This should not happen (inconsistent database)!
        out.emplace_back(make_unique<failure_msg>(message_id, message_type, e.value(), e.message()));
        return cb("", std::move(out));
      }

      // Register bots to send messages to the client.
      for (const auto& bot_id : bots) {
        auto& bot = bots_[bot_id];
        bot->callback_ = std::bind(&bot_manager::sid_cb, this, sid, bot, ph::_1, ph::_2, ph::_3);
        sid_bot_ids_map_[sid].insert(bot_id);
      }

      // Success: send session ID, packages, bots, and bot logs.
      out.emplace_back(make_unique<session_msg>(0, sid));
      out.emplace_back(make_unique<account_msg>(email));
      out.emplace_back(make_unique<packages_msg>(packages_));
      out.emplace_back(make_unique<bots_msg>(get_bot_configs(bots)));
      for (const auto& entry : get_bot_logs(bots)) {
        out.emplace_back(make_unique<update_msg>(entry.first, "log", entry.second));
      }
      return cb(sid, std::move(out));
    });
  });
}

std::map<std::string, std::string> bot_manager::get_bot_configs(
    const std::vector<std::string>& bots) const {
  std::map<std::string, std::string> id_config_map;
  for (const std::string& bot_id : bots) {
    const auto id_match = bots_.find(bot_id);
    if (id_match != bots_.cend()) {
      std::cout << "Reading configuration for " << bot_id << std::endl;
      id_config_map[bot_id] = id_match->second->configuration(false);
    } else {
      std::cerr << "[FATAL] DB and in-memory bot map mismatch: " << bot_id << "\n";
    }
  }
  return id_config_map;
}

std::map<std::string, std::string> bot_manager::get_bot_logs(
    const std::vector<std::string>& bots) const {
  std::map<std::string, std::string> id_log_map;
  for (const std::string& bot_id : bots) {
    const auto id_match = bots_.find(bot_id);
    if (id_match != bots_.cend()) {
      id_log_map[bot_id] = id_match->second->log_msgs();
    } else {
      std::cerr << "[FATAL] DB and in-memory bot map mismatch: " << bot_id << "\n";
    }
  }
  return id_log_map;
}

void bot_manager::sid_cb(
    const std::string& sid,
    std::weak_ptr<bs::bot> bot,
    std::string id, std::string k, std::string v) {
  if (k == "log") {
    std::cout << v;
  }

  std::vector<outgoing_msg_ptr> out;
  out.emplace_back(make_unique<update_msg>(id, k, v));
  sid_callback_(sid, std::move(out));

  std::shared_ptr<bs::bot> locked_bot = bot.lock();
  if (k != "log") {
    std::string::size_type underscore_pos = k.find('_');
    if (underscore_pos != std::string::npos) {
      std::string module = k.substr(0, underscore_pos);
      std::string key = k.substr(underscore_pos + 1);
      config_store_.update_attribute(locked_bot, module, key, v,
                                     [](error_code e) {});
    }
  }
}

void bot_manager::print_cb(
    std::weak_ptr<bs::bot> bot,
    std::string id, std::string k, std::string v) {
  if (k == "log") {
    std::cout << v;
  }

  std::shared_ptr<bs::bot> locked_bot = bot.lock();
  if (k != "log") {
    std::string::size_type underscore_pos = k.find('_');
    if (underscore_pos != std::string::npos) {
      std::string module = k.substr(0, underscore_pos);
      std::string key = k.substr(underscore_pos + 1);
      config_store_.update_attribute(locked_bot, module, key, v,
                                     [](error_code e) {});
    }
  }
}

}  // botscript_server
