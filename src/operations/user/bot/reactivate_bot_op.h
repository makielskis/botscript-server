// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_REACTIVATE_BOT_OP_
#define BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_REACTIVATE_BOT_OP_

#include <vector>
#include <string>

#include "rapidjson/document.h"

#include "./create_bot_op.h"

namespace botscript_server {

/// Bot reactivation message:
/// "Reactivate the bot with the given identifier!"
///
/// {
///   'type': ['user', 'bot', 'create', 'reactivate'],
///   'arguments': {
///     'sid': [String],
///     'identifier': [String],
///     'proxy': [String]
///   }
/// }
class reactivate_bot_op : public create_bot_op {
 public:
  /// Reads the identifier from a reactivate message.
  ///
  /// \param msg the delete message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit reactivate_bot_op(const rapidjson::Document& doc);

  const std::string& identifier() const;
  const std::string& proxy() const;

  virtual std::vector<std::string> type() const override;

 protected:
  virtual std::shared_ptr<botscript::bot_config> bot_config(
      const bs_server& server,
      const user& u) const override;

  virtual void on_load_fail(
      std::shared_ptr<botscript::bot_config> config,
      user u) const override;

  virtual std::string bot_id() const;

 private:
  std::string identifier_;
  std::string proxy_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_REACTIVATE_BOT_OP_
