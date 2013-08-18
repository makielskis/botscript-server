// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./register_msg.h"

namespace botscript_server {

register_msg::register_msg(const rapidjson::Document& doc) {
  username_ = doc["arguments"]["username"].GetString();
  password_ = doc["arguments"]["password"].GetString();
  email_ = doc["arguments"]["email"].GetString();
}

register_msg::register_msg(
    const std::string& username,
    const std::string& password,
    const std::string& email)
    : username_(username),
      password_(password),
      email_(email) {
}

const std::string& register_msg::username() const {
  return username_;
}

const std::string& register_msg::password() const {
  return password_;
}

const std::string& register_msg::email() const {
  return email_;
}

std::vector<std::string> register_msg::type() const {
  return { "register" };
}

}  // namespace botscript_server
