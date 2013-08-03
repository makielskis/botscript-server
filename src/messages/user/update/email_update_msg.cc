// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./email_update_msg.h"

namespace botscript_server {

email_update_msg::email_update_msg(const rapidjson::Document& doc)
    : user_msg(doc),
      current_pw(doc["arguments"]["current_pw"].GetString()),
      new_email(doc["arguments"]["new_email"].GetString()) {
}


}  // namespace botscript_server
