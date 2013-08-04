// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./reactivate_bot_msg.h"

namespace botscript_server {

reactivate_bot_msg::reactivate_bot_msg(const rapidjson::Document& doc)
    : user_msg(doc),
      identifier_(doc["arguments"]["identifier"].GetString()),
      proxy_(doc["arguments"]["proxy"].GetString()) {
}

const std::string& reactivate_bot_msg::identifier() const {
  return identifier_;
}

const std::string& reactivate_bot_msg::proxy() const {
  return proxy_;
}

std::vector<std::string> reactivate_bot_msg::type() const {
  return { "user", "bot", "reactivate" };
}

}  // namespace botscript_server
