// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./email_update_op.h"

#include <iostream> // TODO(felix) remove

namespace botscript_server {

email_update_op::email_update_op(const rapidjson::Document& doc)
    : user_op(doc),
      current_pw_(doc["arguments"]["current_pw"].GetString()),
      new_email_(doc["arguments"]["new_email"].GetString()) {
}

const std::string& email_update_op::current_pw() const {
  return current_pw_;
}

const std::string& email_update_op::new_email() const {
  return new_email_;
}

std::vector<std::string> email_update_op::type() const {
  return {"user", "update", "email"};
}

std::vector<msg_ptr> email_update_op::execute(bs_server& server,
                                              op_callback cb) const {
  std::cout << "Executing email update message" << std::endl;
  return {};
}

}  // namespace botscript_server
