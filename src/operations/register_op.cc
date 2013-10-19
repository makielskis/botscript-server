// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./register_op.h"

namespace botscript_server {

register_op::register_op(const rapidjson::Document& doc) {
  username_ = doc["arguments"]["username"].GetString();
  password_ = doc["arguments"]["password"].GetString();
  email_ = doc["arguments"]["email"].GetString();
}

register_op::register_op(
    const std::string& username,
    const std::string& password,
    const std::string& email)
    : username_(username),
      password_(password),
      email_(email) {
}

const std::string& register_op::username() const {
  return username_;
}

const std::string& register_op::password() const {
  return password_;
}

const std::string& register_op::email() const {
  return email_;
}

std::vector<std::string> register_op::type() const {
  return { "register" };
}

void register_op::execute(bs_server& server, op_callback cb) const {
}

}  // namespace botscript_server
