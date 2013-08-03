// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_ACCOUNT_LOGIN_PASSWORD_LOGIN_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_ACCOUNT_LOGIN_PASSWORD_LOGIN_MSG_

#include <string>

#include "rapidjson/document.h"

namespace botscript_server {

/// Password login message:
/// "Authentificate me with with this username+password"
///
/// {
///   'type': 'login',
///   'action': 'password_login',
///   'arguments': {
///     'username': [String],
///     'password': [String]
///   }
/// }
///
/// Response:
/// If login was successful: success message + session + packages + botlist
/// If login failed: failure message
class password_login_msg {
 public:
  explicit password_login_msg(const rapidjson::Document& doc);

  /// \return the login username
  const std::string& username() const;

  /// \return the login password
  const std::string& password() const;

  /// Checks whether the message is a password login message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is an auth login message
  static bool fits(const rapidjson::Document& doc);

 private:
  std::string username_;
  std::string password_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_ACCOUNT_LOGIN_PASSWORD_LOGIN_MSG_
