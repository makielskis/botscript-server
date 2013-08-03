// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./delete_update_msg.h"

namespace botscript_server {

delete_update_msg::delete_update_msg(const rapidjson::Document& doc)
    : user_msg(doc),
      current_pw_(doc["arguments"]["current_pw"].GetString()) {
}

const std::string& delete_update_msg::current_pw() const {
  return current_pw_;
}

}  // namespace botscript_server
