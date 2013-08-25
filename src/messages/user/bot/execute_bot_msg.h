// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_USER_BOT_EXECUTE_BOT_
#define BOTSCRIPT_SERVER_MESSAGES_USER_BOT_EXECUTE_BOT_

#include <vector>
#include <string>

#include "rapidjson_with_exception.h"

#include "../user_msg.h"

namespace botscript_server {

/// Bot command message:
/// "Let bot XY execute this command with that parameter!"
///
/// {
///   'type': ['user', 'bot', 'execute'],
///   'arguments':  {
///     'sid': [String],
///     'identifier': [String],
///     'execute': {
///       'command': [String],
///       'argument': [String]
///     }
///   }
/// }
class execute_bot_msg : public user_msg {
 public:
  /// Reads the identifier, command and argument from the given message.
  ///
  /// param msg the execute message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit execute_bot_msg(const rapidjson::Document& doc);

  /// \return the identifier of the bot that should execute the command
  const std::string& identifier() const;

  /// \return the command that should be executed
  const std::string& command() const;

  /// \return the argument to execute the command with
  const std::string& argument() const;

  virtual std::vector<std::string> type() const override;

 private:
  std::string identifier_;
  std::string command_;
  std::string argument_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_USER_BOT_EXECUTE_BOT_