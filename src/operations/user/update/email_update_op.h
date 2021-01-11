// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATIONS_USER_UPDATE_EMAIL_UPDATE_OP_
#define BOTSCRIPT_SERVER_OPERATIONS_USER_UPDATE_EMAIL_UPDATE_OP_

#include <vector>
#include <string>

#include "rapidjson/document.h"

#include "../user_op.h"

namespace botscript_server {

/// Update email address message:
/// "Verify password and set my email address to the provided one"
///
/// {
///   'type': ['user', 'update', 'email'],
///   'arguments': {
///     'sid': [String],
///     'current_pw': [String],
///     'new_email': [String]
///   }
/// }
class email_update_op : public user_op {
 public:
  explicit email_update_op(const rapidjson::Document& doc);

  const std::string& current_pw() const;
  const std::string& new_email() const;

  virtual std::vector<std::string> type() const override;
  virtual std::vector<msg_ptr> execute(bs_server& server,
                                       op_callback cb) const override;

 private:
  std::string current_pw_;
  std::string new_email_;
};


}  // namespace botscript_server

#endif   // BOTSCRIPT_SERVER_OPERATIONS_USER_UPDATE_EMAIL_UPDATE_OP_
