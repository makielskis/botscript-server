// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_BOT_MANAGER_H_
#define BOTSCRIPT_SERVER_BOT_MANAGER_H_

#include <vector>
#include <memory>
#include <functional>

#include "boost/asio/io_service.hpp"

#include "./storage/config_store.h"
#include "./storage/user_store.h"

#include "./messages/outgoing/outgoing_msg.h"
#include "./messages/incoming_msgs.h"

namespace botscript {
class bot;
}

namespace botscript_server {

typedef std::unique_ptr<outgoing_msg> outgoing_msg_ptr;
typedef std::function<void (std::vector<outgoing_msg_ptr>)> msg_callback;

class bot_manager {
 public:
  bot_manager(config_store& config_store,
              user_store& user_store,
              boost::asio::io_service* io_service);

  void handle_login_msg(login_msg m, msg_callback cb);
  void handle_user_msg(user_msg m, msg_callback cb);

  void handle_create_bot_msg(create_bot_msg m, msg_callback cb);
  void handle_delete_bot_msg(delete_bot_msg m, msg_callback cb);
  void handle_execute_bot_msg(execute_bot_msg m, msg_callback cb);
  void handle_reactivate_bot_msg(reactivate_bot_msg m, msg_callback cb);

  void handle_delete_update_msg(delete_update_msg m, msg_callback cb);
  void handle_email_update_msg(email_update_msg m, msg_callback cb);
  void handle_password_update_msg(password_update_msg m, msg_callback cb);

 private:
  config_store& config_store_;
  user_store& user_store_;
  boost::asio::io_service* io_service_;
  std::map<std::string, std::shared_ptr<botscript::bot>> bots_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_BOT_MANAGER_H_
