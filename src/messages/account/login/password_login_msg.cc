// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./password_login.h"

namespace botscript_server {

password_login_msg::password_login_msg(const rapidjson::Document& doc) {
  username_ = doc["arguments"]["username"].GetString();
  password_ = doc["arguments"]["password"].GetString();
}

const std::string& password_login_msg::username() const {
  return username_;
}

const std::string& password_login_msg::password() {
  return password_;
}

bool password_login_msg::fits(const rapidjson::Document& doc) {
  return std::string(doc["type"].GetString()) == "login"
         && std::string(doc["action"].GetString()) == "password"
         && doc["arguments"].HasMember("username")
         && doc["arguments"]["username"].IsString()
         && doc["arguments"].HasMember("password")
         && doc["arguments"]["password"].IsString();
}

}  // namespace botscript_server
