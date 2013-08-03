// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OUTGOING_MSG_
#define BOTSCRIPT_SERVER_OUTGOING_MSG_

/// Interface for classes repesenting outgoing messages.
class outgoing_msg {
 public:
  /// \return the message as JSON encoded string
  virtual std::string to_json() const = 0;
};

#endif  // BOTSCRIPT_SERVER_OUTGOING_MSG_

