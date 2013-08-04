// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_OUTGOING_UPDATE_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_OUTGOING_UPDATE_MSG_

#include <string>

#include "./outgoing_msg.h"

namespace botscript_server {

/// Update message:
/// "State change / new log entry for bot XY"
///
/// {
///   'type': 'update',
///   'arguments': {
///     'identifier': [String],
///     'key': [String],
///     'value': [String]
///   }
/// }
class update_msg : public outgoing_msg {
 public:
  /// \param request_id  the ID of the request that was successful
  /// \param request     the type array of the request that was successful
  update_msg(const std::string& id, const std::string& k, const std::string& v);

  virtual std::string to_json() const override;

 private:
  const std::string id_;
  const std::string k_;
  const std::string v_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_OUTGOING_UPDATE_MSG_
