// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./user_op.h"

namespace botscript_server {

user_op::user_op(const std::string& sid)
  : sid_(sid) {
}

user_op::user_op(const rapidjson::Document& doc) {
  sid_ = doc["arguments"]["sid"].GetString();
}

const std::string& user_op::sid() const {
  return sid_;
}

std::vector<std::string> user_op::type() const {
  return { "user" };
}

void user_op::execute(bs_server& server, op_callback cb) const {
}

}  // namespace botscript_server
