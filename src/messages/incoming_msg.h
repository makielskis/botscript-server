// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_INCOMING_MSG_
#define BOTSCRIPT_SERVER_INCOMING_MSG_

#include <vector>
#include <string>

/// Interface for classes repesenting outgoing messages.
class incoming_msg {
 public:
  /// \return the message type array
  virtual std::vector<std::string> type() const = 0;
};

#endif  // BOTSCRIPT_SERVER_INCOMING_MSG_

