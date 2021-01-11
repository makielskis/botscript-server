// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATIONS_USER_UPDATE_DELETE_UPDATE_OP_
#define BOTSCRIPT_SERVER_OPERATIONS_USER_UPDATE_DELETE_UPDATE_OP_

#include <string>
#include <vector>

#include "rapidjson/document.h"

#include "../user_op.h"

namespace botscript_server {

/// Delete account message:
/// "Verify password and delete my account"
///
/// {
///   'type': ['user', 'update', 'delete'],
///   'arguments': {
///     'sid': [String],
///     'current_pw': [String],
///   }
/// }
class delete_update_op : public user_op {
public:
  explicit delete_update_op(const rapidjson::Document& doc);

  /// \return the users current password
  const std::string& current_pw() const;

  virtual std::vector<std::string> type() const override;
  virtual std::vector<msg_ptr> execute(bs_server& server,
                                       op_callback cb) const override;

private:
  /// The users current password
  std::string current_pw_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_OPERATIONS_USER_UPDATE_DELETE_UPDATE_OP_
