// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_REACTIVATE_BOT_OP_
#define BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_REACTIVATE_BOT_OP_

#include <vector>
#include <string>

#include "rapidjson_with_exception.h"

#include "../user_op.h"

namespace botscript_server {

/// Bot reactivation message:
/// "Reactivate the bot with the given identifier!"
///
/// {
///   'type': ['user', 'bot', 'delete'],
///   'arguments': {
///     'sid': [String],
///     'identifier': [String],
///     'proxy': [String]
///   }
/// }
class reactivate_bot_op : public user_op {
 public:
  /// Reads the identifier from a delete message.
  ///
  /// \param msg the delete message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit reactivate_bot_op(const rapidjson::Document& doc);

  /// \return the identifier of the bot to delete
  const std::string& identifier() const;

  /// \return the proxy string
  const std::string& proxy() const;

  virtual std::vector<std::string> type() const override;
  virtual void execute(bs_server& server, op_callback cb) const override;

 private:
  std::string identifier_;
  std::string proxy_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_REACTIVATE_BOT_OP_
