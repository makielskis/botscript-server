// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATIONS_LOGIN_OP_
#define BOTSCRIPT_SERVER_OPERATIONS_LOGIN_OP_

#include "rapidjson/document.h"

#include <string>

#include "./operation.h"

namespace botscript_server {

/// Password login message:
/// "Authentificate me with with this username+password"
///
/// {
///   'type': [ 'login' ],
///   'arguments': {
///     'username': [String],
///     'password': [String]
///   }
/// }
///
/// Response:
/// If login was successful: success message + session + packages + botlist
/// If login failed: failure message
class login_op : public operation {
public:
  explicit login_op(const rapidjson::Document& doc);

  login_op(const std::string& username, const std::string& password);

  /// \return the login username
  const std::string& username() const;

  /// \return the login password
  const std::string& password() const;

  virtual std::vector<std::string> type() const override;
  virtual std::vector<msg_ptr> execute(bs_server& server,
                                       op_callback cb) const override;

private:
  std::string username_;
  std::string password_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_OPERATIONS_LOGIN_OP_
