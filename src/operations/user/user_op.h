// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATIONS_USER_USER_OP_
#define BOTSCRIPT_SERVER_OPERATIONS_USER_USER_OP_

#include <vector>
#include <string>
#include <map>

#include "rapidjson/document.h"

#include "../operation.h"

namespace botscript_server {

class user;

/// This class is used in different cases:
///
/// Session login message:
/// "Authentificate me with this session ID"
///
/// or as a base class for all messages that have a session id
///
/// {
///   'type': ['user'],
///   'arguments': {
///     'sid': [String]
///   }
/// }
class user_op : public operation {
 public:
  virtual ~user_op() {
  }

  /// \param sid the session id
  explicit user_op(const std::string& sid);

  /// Reads the session id from a sid login message.
  ///
  /// \param the create message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit user_op(const rapidjson::Document& doc);

  /// \return the session id
  const std::string& sid() const;

  virtual std::vector<std::string> type() const override;
  virtual std::vector<msg_ptr> execute(bs_server& server,
                                       op_callback cb) const override;

 protected:
  /// Checks whether the session ID is valid.
  /// If it is invalid, this method will throw an session_error.
  ///
  /// \param bs  the bot server to operate on (needed to have DB access)
  /// \return the user that has the session ID contained in this user message
  user get_user_from_session(bs_server& bs) const;

 private:
  std::string sid_;
};

}  // namespace botscript_server

#endif   // BOTSCRIPT_SERVER_OPERATIONS_USER_USER_OP_
