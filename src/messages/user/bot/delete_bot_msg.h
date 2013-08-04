// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_USER_BOT_DELETE_BOT_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_USER_BOT_DELETE_BOT_MSG_

#include <vector>
#include <string>

#include "rapidjson_with_exception.h"

#include "../user_msg.h"

namespace botscript_server {

/// Bot deletion message:
/// "Delete the bot with the given identifier!"
///
/// {
///   'type': ['user', 'bot', 'delete'],
///   'arguments': {
///     'sid': [String],
///     'identifier': [String]
///   }
/// }
class delete_bot_msg : public user_msg {
 public:
  /// Reads the identifier from a delete message.
  ///
  /// \param msg the delete message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit delete_bot_msg(const rapidjson::Document& doc);

  /// \return the identifier of the bot to delete
  const std::string& identifier() const;

  virtual std::vector<std::string> type() const override;

 private:
  std::string identifier_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_USER_BOT_DELETE_BOT_MSG_
