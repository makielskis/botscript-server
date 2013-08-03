// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./delete_bot_msg.h"

namespace botscript_server {

delete_bot_msg::delete_bot_msg(const rapidjson::Document& doc)
    : user_msg(doc),
      identifier_(doc["arguments"]["identifier"].GetString()) {
}

const std::string& delete_bot_msg::identifier() const {
  return identifier_;
}

}  // namespace botscript_server
