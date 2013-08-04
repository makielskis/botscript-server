// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_USER_USER_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_USER_USER_MSG_

#include <vector>
#include <string>

#include "rapidjson_with_exception.h"

#include "../incoming_msg.h"

namespace botscript_server {

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
class user_msg : public incoming_msg {
 public:
  /// Reads the session id from a sid login message.
  ///
  /// \param the create message
  /// \throws parse_exception on parser errors (invalid message format)
  explicit user_msg(const rapidjson::Document& doc);

  /// \return the session id
  const std::string& sid() const;

  virtual std::vector<std::string> type() const override;

 private:
  std::string sid_;
};

}  // namespace botscript_server

#endif   // BOTSCRIPT_SERVER_MESSAGES_USER_USER_MSG_
