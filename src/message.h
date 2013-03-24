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
  std::string sid() const {
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

/// Bot creation message:
/// "Create a bot with the given (valid) configuration!"
///
/// {
///   'type': 'bot_management',
///   'arguments': {
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

  /// \return the bot configuration
  std::string config() const {
    return config_;
  }

  /// Checks whether the message is a create message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is a create message
  static bool fits(const rapidjson::Document& doc);

 private:
  std::string config_;
};

/// Bot deletion message:
/// "Delete the bot with the given identifier!"
///
/// {
///   'type': 'bot_management',
///   'arguments': {
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

  std::string identifier() const {
    return identifier_;
  }

  /// Checks whether the message is a delete message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is a delete message
  static bool fits(const rapidjson::Document& doc);

 private:
  std::string identifier_;
};

/// Bot command message:
/// "Let bot XY execute this command with that parameter!"
///
/// {
///   'type': 'bot',
///   'arguments':  {
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

  /// \return the identifier of the bot that should execute the command
  std::string identifier() const {
    return identifier_;
  }

  /// \return the command that should be executed
  std::string command() const {
    return command_;
  }

  /// \return the argument to execute the command with
  std::string argument() const {
    return argument_;
  }

  /// Checks whether the message is a execute message.
  /// We assume that the general message format was already checked.
  ///
  /// \param doc the document to check
  /// \return whether the documents is a execute message
  static bool fits(const rapidjson::Document& doc);

 private:
  std::string identifier_;
  std::string command_;
  std::string argument_;
};

}  // namespace botscript_server

#endif   // BOTSERVER_MESSAGE_H_
