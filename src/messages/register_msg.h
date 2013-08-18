// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_REGISTER_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_REGISTER_MSG_

#include <string>

#include "./incoming_msg.h"
#include "./rapidjson_with_exception.h"

namespace botscript_server {

/// Password login message:
/// "Authentificate me with with this username+password"
///
/// {
///   'type': [ 'register' ],
///   'arguments': {
///     'username': [String],
///     'password': [String],
///     'email': [String]
///   }
/// }
///
/// Response:
/// If registration was successful: success message
/// If registration failed: failure message
class register_msg : public incoming_msg {
 public:
  explicit register_msg(const rapidjson::Document& doc);

  register_msg(
      const std::string& username,
      const std::string& password,
      const std::string& email);

  /// \return the account username
  const std::string& username() const;

  /// \return the account password
  const std::string& password() const;

  /// \return the registration email address
  const std::string& email() const;

  virtual std::vector<std::string> type() const override;

 private:
  std::string username_;
  std::string password_;
  std::string email_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_REGISTER_MSG_
