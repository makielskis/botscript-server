// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATION_MSG_
#define BOTSCRIPT_SERVER_OPERATION_MSG_

#include <vector>
#include <string>

#include "../messages/message.h"

namespace botscript_server {

class bs_server;

typedef std::unique_ptr<message> msg_ptr;
typedef std::function<void (std::string, std::vector<msg_ptr>)> op_callback;

/// Interface for classes repesenting outgoing messages.
class operation {
 public:
  /// \return the message type array
  virtual std::vector<std::string> type() const = 0;

  /// Executes this operation on the given botscript server
  ///
  /// \param server  the server to execute this action on
  virtual void execute(bs_server& server, op_callback cb) const = 0;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_OPERATION_MSG_

