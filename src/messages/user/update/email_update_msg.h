// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_EMAIL_UPDATE_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_EMAIL_UPDATE_MSG_

#include <string>

#include "./user_msg.h"

#include "../rapidjson_with_exception.h"

namespace botscript_server {

/// Update email address message:
/// "Verify password and set my email address to the provided one"
///
/// {
///   'type': ['user', 'update', 'email'],
///   'arguments': {
///     'sid': [String],
///     'current_pw': [String],
///     'new_email': [String]
///   }
/// }
class email_update_msg : public user_msg {
 public:
  explicit email_update_msg(const rapidjson::Document& doc);

  const std::string& current_pw() const;
  const std::string& new_email() const;

 private:
  std::string current_pw;
  std::string new_email;
};


}  // namespace botscript_server

#endif   // BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_EMAIL_UPDATE_MSG_
