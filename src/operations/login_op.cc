// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./login_op.h"

#include "../make_unique.h"

#include "../messages/failure_msg.h"

#include <iostream>  // TODO(felix) remove

namespace botscript_server {

login_op::login_op(const rapidjson::Document& doc) {
  username_ = doc["arguments"]["username"].GetString();
  password_ = doc["arguments"]["password"].GetString();
}

login_op::login_op(const std::string& username, const std::string& password)
    : username_(username),
      password_(password) {
}

const std::string& login_op::username() const {
  return username_;
}

const std::string& login_op::password() const {
  return password_;
}

std::vector<std::string> login_op::type() const {
  return { "login" };
}

void login_op::execute(bs_server& server, op_callback cb) const {
  std::cout << "Executing login operation.\n";
  std::vector<msg_ptr> out;
  out.emplace_back(make_unique<failure_msg>(0, type(), 1, ""));
  return cb("", std::move(out));
}

}  // namespace botscript_server
