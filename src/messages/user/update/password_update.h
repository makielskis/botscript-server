// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_PASSWORD_UPDATE_MSG
#define BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_PASSWORD_UPDATE_MSG

#include <string>

#include "./user_msg.h"

#include "../../rapidjson_with_exception.h"

namespace botscript_server {

/// Update password message:
/// "Verify password and set my password address to the provided new one"
///
/// {
///   'type': ['user', 'update', 'password'],
///   'arguments': {
///     'sid': [String],
///     'current_pw': [String],
///     'new_pw': [String]
///   }
/// }
class password_update_msg : public user_msg {
 public:
  explicit password_update_msg(const rapidjson::Document& doc);

  const std::string& current_pw() const;
  const std::string& new_pw() const;

 private:
  std::string current_pw;
  std::string new_pw;
};


}  // namespace botscript_server

#endif   // BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_PASSWORD_UPDATE_MSG
