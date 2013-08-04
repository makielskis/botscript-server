// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_OUTGOING_failure_msg_
#define BOTSCRIPT_SERVER_MESSAGES_OUTGOING_failure_msg_

#include <string>
#include <vector>

#include "./outgoing_msg.h"

namespace botscript_server {

/// Failure message:
/// "Request XY failed"
///
/// {
///   'type': 'failure',
///   'arguments': {
///     request: [Array of Strings]
///     request_id: [Integer],
///     error_code: [Integer],
///     reason: [String]
///   }
/// }
class failure_msg : public outgoing_msg {
 public:
  /// \param request_id  the ID of the request that was successful
  /// \param request     the type array of the request that was successful
  /// \param error_code  error code that corresponds to the failure
  /// \param reason      string describing the reason of the error
  failure_msg(unsigned int request_id,
              const std::vector<std::string>& request,
              unsigned int error_code,
              const std::string& reason);

  virtual std::string to_json() const;

 private:
  /// The ID of the request that was successful
  const unsigned int request_id_;

  /// The type array of the request that was successful
  const std::vector<std::string> request_;

  /// The error code that corresponds to the failure
  const unsigned int error_code_;

  /// A string describing the reason of the error
  const std::string reason_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_OUTGOING_failure_msg_
