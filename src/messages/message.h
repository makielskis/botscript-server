// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_MESSAGE_
#define BOTSCRIPT_SERVER_MESSAGES_MESSAGE_

#include <vector>
#include <functional>
#include <memory>

/// Interface for classes repesenting outgoing messages.
class message {
 public:
  /// \return the message as JSON encoded string
  virtual std::string to_json() const = 0;

  virtual ~message() {
  }
};

#endif  // BOTSCRIPT_SERVER_MESSAGES_MESSAGE_

