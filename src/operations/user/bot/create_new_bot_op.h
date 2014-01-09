// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_CREATE_NEW_BOT_OP_
#define BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_CREATE_NEW_BOT_OP_

#include "create_bot_op.h"

namespace botscript_server {

/// Operation creating a new bot:
/// "Create a bot with the given configuration!"
///
/// {
///   'type': ['user', 'bot', 'create', 'new'],
///   'arguments': {
///     'sid': [String],
///     'config': [String]
///   }
/// }
class create_new_bot_op : public create_bot_op {
 public:
  /// \param sid     the session id
  /// \param config  the configuration to load the bot with
  create_new_bot_op(const std::string& sid, const std::string& config);

  /// Reads the identifier from a reactivate message.
  ///
  /// \param msg the delete message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit create_new_bot_op(const rapidjson::Document& doc);

  const std::string& config() const;

  virtual std::vector<std::string> type() const override;

 protected:
  virtual std::shared_ptr<botscript::bot_config> bot_config(
      const bs_server& server,
      const user& u) const override;

  virtual void on_load_fail(
      std::shared_ptr<botscript::bot_config> config,
      user u) const override;

  virtual std::string bot_id() const override;

 private:
  std::string config_;
};

}  // namespace botscritp_server

#endif  // BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_CREATE_NEW_BOT_OP_
