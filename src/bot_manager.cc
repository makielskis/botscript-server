// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./bot_manager.h"

#include <utility>

#include "./messages/outgoing_msgs.h"

using boost::system::error_code;

namespace botscript_server {

bot_manager::bot_manager(config_store& config_store,
                         user_store& user_store,
                         boost::asio::io_service* io_service)
    : config_store_(config_store),
      user_store_(user_store),
      io_service_(io_service) {
}

void bot_manager::handle_login_msg(
    login_msg m,
    msg_callback cb) {
  user_store_.login(m.username(), m.password(), [=](std::string, error_code e) {
    std::vector<outgoing_msg_ptr> out;
    if (e) {
      std::vector<outgoing_msg_ptr> response;
      auto msg_ptr = new failure_msg(0, m.type(), e.value(), e.message());
      auto msg_smart_ptr = std::unique_ptr<outgoing_msg>(msg_ptr);
      response.emplace_back(std::move(msg_smart_ptr));
      return cb(std::move(response));
    } else {
      // TODO(felix) implement success case: send session ID, bots, packages, logs, ...
    }
  });
}

void bot_manager::handle_user_msg(
    user_msg m,
    msg_callback cb) {
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

}  // botscript_server
