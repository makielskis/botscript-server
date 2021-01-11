// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_SESSION_H_
#define BOTSCRIPT_SERVER_SESSION_H_

#include <set>
#include <string>

#include "./user.h"

namespace botscript_server {

/// Structure holding the information for a session:
/// Session ID and the corresponding user (containing also the expire time)
struct session {
  /// The unique session ID
  std::string id;

  /// The user that owns this session
  user u;

  /// Comparison using the ID
  ///
  /// \param o  the other session to compare with
  /// \return id < o.id
  bool operator<(const session& o) { return id < o.id; }
};

#endif  // BOTSCRIPT_SERVER_SESSION_H_

}  // namespace botscript_server
