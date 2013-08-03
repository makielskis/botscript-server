// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./user_msg.h"

namespace botscript_server {

user_msg::user_msg(const rapidjson::Document& doc) {
  sid_ = doc["arguments"]["sid"].GetString();
}

const std::string& user_msg::sid() const {
  return sid_;
}

}  // namespace botscript_server
