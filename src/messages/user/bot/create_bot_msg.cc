// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./create_bot_msg.h"

namespace botscript_server {

create_bot_msg::create_bot_msg(const std::string& sid,
                               const std::string& config)
    : user_msg(sid),
      config_(config) {
}

create_bot_msg::create_bot_msg(const rapidjson::Document& doc)
    : user_msg(doc),
      config_(doc["arguments"]["config"].GetString()) {
}

const std::string& create_bot_msg::config() const {
  return config_;
}

std::vector<std::string> create_bot_msg::type() const {
  return { "user", "bot", "create" };
}

}  // namespace botscript_server
