// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./create_bot_op.h"

namespace botscript_server {

create_bot_op::create_bot_op(const std::string& sid,
                             const std::string& config)
    : user_op(sid),
      config_(config) {
}

create_bot_op::create_bot_op(const rapidjson::Document& doc)
    : user_op(doc),
      config_(doc["arguments"]["config"].GetString()) {
}

const std::string& create_bot_op::config() const {
  return config_;
}

std::vector<std::string> create_bot_op::type() const {
  return { "user", "bot", "create" };
}

void create_bot_op::execute(bs_server& server, op_callback cb) const {
}

}  // namespace botscript_server
