// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSERVER_MESSAGE_H_
#define BOTSERVER_MESSAGE_H_

#include <string>
#include <map>
#include <utility>
#include <stdexcept>

#include "bot.h"

namespace botscript_server {

/// Exception that gets thrown on parser errors.
class parse_exception : public std::runtime_error {
 public:
  parse_exception()
    : std::runtime_error("invalid json") {
  }
};

/// Login login message:
/// "Authentificate me with with this username+password"
///
/// {
///   'type': 'login',
///   'arguments': {
///     'username': [String],
///     'password': [String]
///   }
/// }
class auth_login_message {
 public:
  explicit auth_login_message(const rapidjson::Document& doc);

  /// \return the login username
  const std::string& username() const {
    return username_;
  }

  /// \return the login password
  const std::string& password() const {
    return password_;
  }

  /// Checks whether the message is a auth login message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is an auth login message
  static bool fits(const rapidjson::Document& doc);

 private:
  std::string username_;
  std::string password_;
};

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

/// Abstract account update message:
/// "Update account setting 'key' (i.e. email or password)"
///
/// {
///   'type': 'account',
///   'arguments': {
///     'sid': [String],
///     'password': [String],
///     'update': {
///       'key': [String],
///        ...
///     }
///   }
/// }
class update_account_message {
 public:
  explicit update_account_message(const rapidjson::Document& doc);

  /// \return the users session id
  const std::string& sid() const {
    return sid_;
  }

  /// \return the current password
  const std::string& password() const {
    return password_;
  }

  /// Checks whether the message is an update message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is a update message
  static bool fits(const rapidjson::Document& doc);

 private:
  std::string sid_;
  std::string password_;
};

/// Account deletion message:
/// "Delete my account"
///
/// {
///   'type': 'account',
///   'arguments': {
///     'sid': [String],
///     'password': [String],
///     'update': {
///       'key': 'delete'
///     }
///   }
/// }
class delete_account_message : public update_account_message {
 public:
  explicit delete_account_message(const rapidjson::Document& doc);

  /// Checks whether the message is an account deletion message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is an account deletion message
  static bool fits(const rapidjson::Document& doc);
};

/// Email address update message:
/// "Update my email address"
///
/// {
///   'type': 'account',
///   'arguments': {
///     'sid': [String],
///     'password': [String],
///     'update': {
///       'key': 'email',
///       'value': [String]
///     }
///   }
/// }
class set_email_message : public update_account_message {
 public:
  explicit set_email_message(const rapidjson::Document& doc);

  /// \return the new email address to set
  const std::string& new_email() const {
    return new_email_;
  }

  /// Checks whether the message is a mail update message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is an account mail update
  static bool fits(const rapidjson::Document& doc);

 private:
  std::string new_email_;
};

/// Password update message:
/// "Update my password"
///
/// {
///   'type': 'account',
///   'arguments': {
///     'sid': [String],
///     'password': [String],
///     'update': {
///       'key': 'password',
///       'value': [String]
///     }
///   }
/// }
class set_password_message : public update_account_message {
 public:
  explicit set_password_message(const rapidjson::Document& doc);

  /// \return the new password to set
  const std::string& new_password() const {
    return new_password_;
  }

  /// Checks whether the message is a password update message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is a password update message
  static bool fits(const rapidjson::Document& doc);

 private:
  std::string new_password_;
};

/// Bot creation message:
/// "Create a bot with the given (valid) configuration!"
///
/// {
///   'type': 'bot_management',
///   'arguments': {
///     'sid': [String],
///     'type': 'create',
///     'config': [String]
///   }
/// }
class create_message {
 public:
  /// Reads the configuration from a create message.
  ///
  /// \param the create message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit create_message(const rapidjson::Document& doc);

  /// \return the users session id
  const std::string& sid() const {
    return sid_;
  }

  /// \return the bot configuration
  const std::string& config() const {
    return config_;
  }

  /// Checks whether the message is a create message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is a create message
  static bool fits(const rapidjson::Document& doc);

 private:
  std::string sid_;
  std::string config_;
};

/// Bot deletion message:
/// "Delete the bot with the given identifier!"
///
/// {
///   'type': 'bot_management',
///   'arguments': {
///     'sid': [String],
///     'type': 'delete',
///     'identifier': [String]
///   }
/// }
class delete_message {
 public:
  /// Reads the identifier from a delete message.
  ///
  /// \param msg the delete message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit delete_message(const rapidjson::Document& doc);

  /// \return the users session id
  const std::string& sid() const {
    return sid_;
  }

  /// \return the identifier of the bot to delete
  const std::string& identifier() const {
    return identifier_;
  }

  /// Checks whether the message is a delete message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is a delete message
  static bool fits(const rapidjson::Document& doc);

 private:
  std::string sid_;
  std::string identifier_;
};

/// Bot command message:
/// "Let bot XY execute this command with that parameter!"
///
/// {
///   'type': 'bot',
///   'arguments':  {
///     'sid': [String],
///     'identifier': [String],
///     'execute': {
///       'command': [String],
///       'argument': [String]
///     }
///   }
/// }
class execute_message {
 public:
  /// Reads the identifier, command and argument from the given message.
  ///
  /// param msg the execute message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit execute_message(const rapidjson::Document& doc);

  /// \return the users session id
  const std::string& sid() const {
    return sid_;
  }

  /// \return the identifier of the bot that should execute the command
  const std::string& identifier() const {
    return identifier_;
  }

  /// \return the command that should be executed
  const std::string& command() const {
    return command_;
  }

  /// \return the argument to execute the command with
  const std::string& argument() const {
    return argument_;
  }

  /// Checks whether the message is a execute message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is a execute message
  static bool fits(const rapidjson::Document& doc);

 private:
  std::string sid_;
  std::string identifier_;
  std::string command_;
  std::string argument_;
};

}  // namespace botscript_server

#endif   // BOTSERVER_MESSAGE_H_
