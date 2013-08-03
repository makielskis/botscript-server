// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./execute_bot_msg.h"

namespace botscript_server {

execute_message::execute_message(const rapidjson::Document& doc)
    : user_msg(doc),
      identifier_(doc["arguments"]["identifier"].GetString()),
      command_(doc["arguments"]["execute"]["command"].GetString()),
      argument_(doc["arguments"]["execute"]["argument"].GetString()) {
}

const std::string& execute_message::identifier() const {
  return identifier_;
}

const std::string& execute_message::command() const {
  return command_;
}

const std::string& execute_message::argument() const {
  return argument_;
}

}  // namespace botscript_server
