// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./execute_bot_msg.h"

namespace botscript_server {

execute_bot_msg::execute_bot_msg(const rapidjson::Document& doc)
    : user_msg(doc),
      identifier_(doc["arguments"]["identifier"].GetString()),
      command_(doc["arguments"]["execute"]["command"].GetString()),
      argument_(doc["arguments"]["execute"]["argument"].GetString()) {
}

const std::string& execute_bot_msg::identifier() const {
  return identifier_;
}

const std::string& execute_bot_msg::command() const {
  return command_;
}

const std::string& execute_bot_msg::argument() const {
  return argument_;
}

std::vector<std::string> execute_bot_msg::type() const {
  return { "user", "bot", "execute" };
}

}  // namespace botscript_server
