// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./password_update_msg.h"

namespace botscript_server {

password_update_msg::password_update_msg(const rapidjson::Document& doc)
    : user_msg(doc),
      current_pw(doc["arguments"]["current_pw"].GetString()),
      new_pw(doc["arguments"]["new_pw"].GetString()) {
}


}  // namespace botscript_server
