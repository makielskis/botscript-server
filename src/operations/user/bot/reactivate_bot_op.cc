// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./reactivate_bot_op.h"

namespace botscript_server {

reactivate_bot_op::reactivate_bot_op(const rapidjson::Document& doc)
    : user_op(doc),
      identifier_(doc["arguments"]["identifier"].GetString()),
      proxy_(doc["arguments"]["proxy"].GetString()) {
}

const std::string& reactivate_bot_op::identifier() const {
  return identifier_;
}

const std::string& reactivate_bot_op::proxy() const {
  return proxy_;
}

std::vector<std::string> reactivate_bot_op::type() const {
  return { "user", "bot", "reactivate" };
}

void reactivate_bot_op::execute(bs_server& server, op_callback cb) const {
}

}  // namespace botscript_server
