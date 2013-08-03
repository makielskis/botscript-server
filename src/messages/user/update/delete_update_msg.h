// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_DELETE_UPDATE_MSG
#define BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_DELETE_UPDATE_MSG

#include <string>

#include "./user_msg.h"

#include "../rapidjson_with_exception.h"

namespace botscript_server {

/// Delete account message:
/// "Verify password and delete my account"
///
/// {
///   'type': ['user', 'update', 'delete'],
///   'arguments': {
///     'sid': [String],
///     'current_pw': [String],
///   }
/// }
class password_update_msg : public user_msg {
 public:
  explicit password_update_msg(const rapidjson::Document& doc);

  const std::string& current_pw() const;

 private:
  std::string current_pw;
};


}  // namespace botscript_server

#endif   // BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_DELETE_UPDATE_MSG
