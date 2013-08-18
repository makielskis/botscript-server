// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./bot_manager.h"

#include <iostream>
#include <utility>

#include "bot.h"

#include "./messages/outgoing_msgs.h"

using boost::system::error_code;
namespace bs = botscript;

namespace botscript_server {

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

bot_manager::bot_manager(config_store& config_store,
                         user_store& user_store,
                         boost::asio::io_service* io_service)
    : config_store_(config_store),
      user_store_(user_store),
      io_service_(io_service),
      packages_(bs::bot::load_packages("../test/packages")) {
}

void bot_manager::handle_login_msg(
    login_msg m,
    msg_callback cb) {
  user_store_.login(m.username(), m.password(), [=](std::string sid, error_code e) {
    if (e) {
      // Failure: responde with failure message.
      std::vector<outgoing_msg_ptr> out;
      out.emplace_back(make_unique<failure_msg>(0, m.type(), e.value(), e.message()));
      return cb(std::move(out));
    }

    return on_login(sid, 0, m.type(), cb);
  });
}

void bot_manager::handle_user_msg(
    user_msg m,
    msg_callback cb) {
  return on_login(m.sid(), 0, m.type(), cb);
}

void bot_manager::handle_create_bot_msg(
    create_bot_msg m,
    msg_callback cb) {
}

void bot_manager::handle_delete_bot_msg(
    delete_bot_msg m,
    msg_callback cb) {
}

void bot_manager::handle_execute_bot_msg(
    execute_bot_msg m,
    msg_callback cb) {
}

void bot_manager::handle_reactivate_bot_msg(
    reactivate_bot_msg m,
    msg_callback cb) {
}

void bot_manager::handle_delete_update_msg(
    delete_update_msg m,
    msg_callback cb) {
}

void bot_manager::handle_email_update_msg(
    email_update_msg m,
    msg_callback cb) {
}

void bot_manager::handle_password_update_msg(
    password_update_msg m,
    msg_callback cb) {
}

void bot_manager::on_login(
    std::string sid,
    int message_id,
    std::vector<std::string> message_type,
    msg_callback cb) {
  return user_store_.get_email(sid, [=](std::string email, error_code e) {
    if (e) {
      // Failure: respond with failure message.
      std::vector<outgoing_msg_ptr> out;
      out.emplace_back(make_unique<failure_msg>(message_id, message_type, e.value(), e.message()));
      return cb(std::move(out));
    }

    // Success: get bots that are owned by this user.
    return user_store_.get_bots(sid, [=](std::vector<std::string> bots, error_code e) {
      std::vector<outgoing_msg_ptr> out;

      if (e) {
        // This should not happen (inconsistent database)!
        out.emplace_back(make_unique<failure_msg>(message_id, message_type, e.value(), e.message()));
        return cb(std::move(out));
      }

      // Success: send session ID, packages, bots, and bot logs.
      out.emplace_back(make_unique<session_msg>(0, sid));
      out.emplace_back(make_unique<account_msg>(email));
      out.emplace_back(make_unique<packages_msg>(packages_));
      out.emplace_back(make_unique<bots_msg>(get_bot_configs(bots)));
      for (const auto& entry : get_bot_logs(bots)) {
        out.emplace_back(make_unique<update_msg>(entry.first, "log", entry.second));
      }
      return cb(std::move(out));
    });
  });
}

std::map<std::string, std::string> bot_manager::get_bot_configs(
    const std::vector<std::string>& bots) const {
  std::map<std::string, std::string> id_config_map;
  for (const std::string& bot_id : bots) {
    const auto id_match = bots_.find(bot_id);
    if (id_match != bots_.cend()) {
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

}  // botscript_server
