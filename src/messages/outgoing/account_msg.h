// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_OUTGOING_ACCOUNT_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_OUTGOING_ACCOUNT_MSG_

#include <string>

#include "./outgoing_msg.h"

namespace botscript_server {

/// Account message:
/// "These are your account details (current email address)"
///
/// {
///   'type': 'account',
///   'arguments': {
///     email: [String]
///   }
/// }
class account_msg : public outgoing_msg {
 public:
  /// \param email  email address
  account_msg(const std::string& email);

  virtual std::string to_json() const override;

 private:
  /// Email address
  const std::string email_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_OUTGOING_ACCOUNT_MSG_
