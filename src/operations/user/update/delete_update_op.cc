// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./delete_update_op.h"

#include <iostream> // TODO(felix) remove

namespace botscript_server {

delete_update_op::delete_update_op(const rapidjson::Document& doc)
    : user_op(doc),
      current_pw_(doc["arguments"]["current_pw"].GetString()) {
}

const std::string& delete_update_op::current_pw() const {
  return current_pw_;
}

std::vector<std::string> delete_update_op::type() const {
  return {"user", "update", "delete"};
}

std::vector<msg_ptr> delete_update_op::execute(bs_server& server,
                                               op_callback cb) const {
  std::cout << "Executing delete update message" << std::endl;
  return {};
}

}  // namespace botscript_server
