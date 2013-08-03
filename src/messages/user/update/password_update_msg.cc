// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./password_update_msg.h"

namespace botscript_server {

password_update_msg::password_update_msg(const rapidjson::Document& doc)
    : user_msg(doc),
      current_pw_(doc["arguments"]["current_pw"].GetString()),
      new_pw_(doc["arguments"]["new_pw"].GetString()) {
}

const std::string& password_update_msg::current_pw() const {
  return current_pw_;
}

const std::string& password_update_msg::new_pw() const {
  return new_pw_;
}

}  // namespace botscript_server
