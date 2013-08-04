// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_LOGIN_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_LOGIN_MSG_

#include <string>

#include "./incoming_msg.h"
#include "./rapidjson_with_exception.h"

namespace botscript_server {

/// Password login message:
/// "Authentificate me with with this username+password"
///
/// {
///   'type': [ 'login' ],
///   'arguments': {
///     'username': [String],
///     'password': [String]
///   }
/// }
///
/// Response:
/// If login was successful: success message + session + packages + botlist
/// If login failed: failure message
class login_msg : public incoming_msg {
 public:
  explicit login_msg(const rapidjson::Document& doc);

  /// \return the login username
  const std::string& username() const;

  /// \return the login password
  const std::string& password() const;

  virtual std::vector<std::string> type() const override;

 private:
  std::string username_;
  std::string password_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_LOGIN_MSG_
