// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_CREATE_BOT_OP_
#define BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_CREATE_BOT_OP_

#include <vector>
#include <string>

#include "rapidjson_with_exception.h"

#include "../user_op.h"

namespace botscript_server {

/// Bot creation message:
/// "Create a bot with the given configuration!"
///
/// {
///   'type': ['user', 'bot', 'create'],
///   'arguments': {
///     'sid': [String],
///     'config': [String]
///   }
/// }
class create_bot_op : public user_op {
 public:
  /// \param sid     the session id
  /// \param config  the configuration
  create_bot_op(const std::string& sid, const std::string& config);

  /// Reads the configuration from a create message.
  ///
  /// \param the create message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit create_bot_op(const rapidjson::Document& doc);

  /// \return the bot configuration
  const std::string& config() const;

  virtual std::vector<std::string> type() const override;
  virtual void execute(bs_server& server, op_callback cb) const override;

 private:
  std::string config_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_CREATE_BOT_OP_
