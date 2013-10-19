// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./password_update_op.h"

namespace botscript_server {

password_update_op::password_update_op(const rapidjson::Document& doc)
    : user_op(doc),
      current_pw_(doc["arguments"]["current_pw"].GetString()),
      new_pw_(doc["arguments"]["new_pw"].GetString()) {
}

const std::string& password_update_op::current_pw() const {
  return current_pw_;
}

const std::string& password_update_op::new_pw() const {
  return new_pw_;
}

std::vector<std::string> password_update_op::type() const {
  return { "user", "update", "password" };
}

void password_update_op::execute(bs_server& server, op_callback cb) const {
}

}  // namespace botscript_server