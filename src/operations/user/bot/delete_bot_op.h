// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_DELETE_BOT_OP_
#define BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_DELETE_BOT_OP_

#include <vector>
#include <string>

#include "rapidjson/document.h"

#include "../user_op.h"

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
class delete_bot_op : public user_op {
 public:
  /// \param sid         the session id
  /// \param identifier  the id if the bot to delete
  delete_bot_op(const std::string& sid, const std::string& identifier);

  /// Reads the identifier from a delete message.
  ///
  /// \param msg the delete message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit delete_bot_op(const rapidjson::Document& doc);

  /// \return the identifier of the bot to delete
  const std::string& identifier() const;

  virtual std::vector<std::string> type() const override;
  virtual std::vector<msg_ptr> execute(bs_server& server,
                                       op_callback cb) const override;

 private:
  std::string identifier_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_DELETE_BOT_OP_
