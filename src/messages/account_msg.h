// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_ACCOUNT_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_ACCOUNT_MSG_

#include <string>

#include "./message.h"

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
class account_msg : public message {
 public:
  /// \param email  email address
  account_msg(const std::string& email);

  /// \return the account email address
  const std::string& email() const;

  virtual std::string to_json() const override;

 private:
  /// Email address
  const std::string email_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_ACCOUNT_MSG_
