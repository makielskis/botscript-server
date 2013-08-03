// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./password_login_msg.h"

namespace botscript_server {

password_login_msg::password_login_msg(const rapidjson::Document& doc) {
  username_ = doc["arguments"]["username"].GetString();
  password_ = doc["arguments"]["password"].GetString();
}

const std::string& password_login_msg::username() const {
  return username_;
}

const std::string& password_login_msg::password() const {
  return password_;
}

}  // namespace botscript_server
