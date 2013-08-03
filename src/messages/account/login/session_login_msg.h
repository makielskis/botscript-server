// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_ACCOUNT_LOGIN_PASSWORD_LOGIN_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_ACCOUNT_LOGIN_PASSWORD_LOGIN_MSG_

namespace botscript_server {

/// Session login message:
/// "Authentificate me with this session ID"
///
/// {
///   'type': 'login',
///   'arguments': {
///     'sid': [String]
///   }
/// }
class sid_login_message {
 public:
  /// Reads the session id from a sid login message.
  ///
  /// \param the create message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit sid_login_message(const rapidjson::Document& doc);

  /// \return the session id
  const std::string& sid() const {
    return sid_;
  };

  /// Checks whether the message is a sid login message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is a sid login message
  static bool fits(const rapidjson::Document& doc);

 private:
  std::string sid_;
};
