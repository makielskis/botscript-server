// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./email_update_msg.h"

namespace botscript_server {

email_update_msg::email_update_msg(const rapidjson::Document& doc)
    : user_msg(doc),
      current_pw_(doc["arguments"]["current_pw"].GetString()),
      new_email_(doc["arguments"]["new_email"].GetString()) {
}

const std::string& email_update_msg::current_pw() const {
  return current_pw_;
}

const std::string& email_update_msg::new_email() const {
  return new_email_;
}

std::vector<std::string> email_update_msg::type() const {
  return { "user", "update", "email" };
}

}  // namespace botscript_server
