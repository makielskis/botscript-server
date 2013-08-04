// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_OUTGOING_SESSION_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_OUTGOING_SESSION_MSG_

#include <ctime>
#include <string>

#include "./outgoing_msg.h"

namespace botscript_server {

/// Session message:
/// "This your session ID."
///
/// {
///   'type': 'session',
///   'arguments': {
///     sid: [String],
///     expire: [Number]
///   }
/// }
class session_msg : public outgoing_msg {
 public:
  /// \param expire  session expiration timestamp
  /// \param sid     session ID
  session_msg(const time_t expire, const std::string& sid);

  virtual std::string to_json() const override;

 private:
  /// Session expiration timestamp
  const time_t expire_;

  /// Session ID
  const std::string sid_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_OUTGOING_SESSION_MSG_
