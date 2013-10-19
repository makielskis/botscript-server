// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_FAILURE_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_FAILURE_MSG_

#include <string>
#include <vector>

#include "./message.h"

namespace botscript_server {

/// Failure message:
/// "Request XY failed"
///
/// {
///   'type': 'failure',
///   'arguments': {
///     request: [Array of Strings],
///     request_id: [Integer],
///     error_code: [Integer],
///     reason: [String]
///   }
/// }
class failure_msg : public message {
 public:
  /// \param request_id  the ID of the request that was successful
  /// \param request     the type array of the request that was successful
  /// \param error_code  error code that corresponds to the failure
  /// \param reason      string describing the reason of the error
  failure_msg(unsigned int request_id,
              const std::vector<std::string>& request,
              unsigned int error_code,
              const std::string& reason);

  virtual std::string to_json() const override;

  /// \return the request id
  unsigned int request_id() const;

  /// \return the request type
  const std::vector<std::string>& request() const;

  /// \return the error code
  unsigned int error_code() const;

  /// \retrun the error reason
  const std::string& reason() const;

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

#endif  // BOTSCRIPT_SERVER_MESSAGES_FAILURE_MSG_
