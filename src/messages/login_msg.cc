// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./login_msg.h"

namespace botscript_server {

login_msg::login_msg(const rapidjson::Document& doc) {
  username_ = doc["arguments"]["username"].GetString();
  password_ = doc["arguments"]["password"].GetString();
}

login_msg::login_msg(const std::string& username, const std::string& password)
    : username_(username),
      password_(password) {
}

const std::string& login_msg::username() const {
  return username_;
}

const std::string& login_msg::password() const {
  return password_;
}

std::vector<std::string> login_msg::type() const {
  return { "login" };
}

}  // namespace botscript_server
