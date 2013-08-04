// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_DELETE_UPDATE_MSG
#define BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_DELETE_UPDATE_MSG

#include <vector>
#include <string>

#include "rapidjson_with_exception.h"

#include "../user_msg.h"

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
class delete_update_msg : public user_msg {
 public:
  explicit delete_update_msg(const rapidjson::Document& doc);

  /// \return the users current password
  const std::string& current_pw() const;

  virtual std::vector<std::string> type() const override;

 private:
  /// The users current password
  std::string current_pw_;
};


}  // namespace botscript_server

#endif   // BOTSCRIPT_SERVER_MESSAGES_USER_UPDATE_DELETE_UPDATE_MSG
