// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_PASSWORD_UPDATE_MSG
#define BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_PASSWORD_UPDATE_MSG

#include <vector>
#include <string>

#include "rapidjson_with_exception.h"

#include "../user_msg.h"

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

  virtual std::vector<std::string> type() const override;

 private:
  std::string current_pw_;
  std::string new_pw_;
};


}  // namespace botscript_server

#endif   // BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_PASSWORD_UPDATE_MSG