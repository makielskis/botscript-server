// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATIONS_USER_UPDATE_PASSWORD_UPDATE_OP_
#define BOTSCRIPT_SERVER_OPERATIONS_USER_UPDATE_PASSWORD_UPDATE_OP_

#include <string>
#include <vector>

#include "rapidjson/document.h"

#include "../user_op.h"

namespace botscript_server {

/// Update password message:
/// "Verify password and set my password address to the provided new one"
///
/// {
///   'type': ['user', 'update', 'password'],
///   'arguments': {
///     'sid': [String],
///     'current_pw': [String],
///     'new_pw': [String]
///   }
/// }
class password_update_op : public user_op {
public:
  explicit password_update_op(const rapidjson::Document& doc);

  const std::string& current_pw() const;
  const std::string& new_pw() const;

  virtual std::vector<std::string> type() const override;
  virtual std::vector<msg_ptr> execute(bs_server& server,
                                       op_callback cb) const override;

private:
  std::string current_pw_;
  std::string new_pw_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_OPERATIONS_USER_UPDATE_PASSWORD_UPDATE_OP_
