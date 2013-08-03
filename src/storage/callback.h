// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef CALLBACK_H_
#define CALLBACK_H_

namespace botscript_server {

/// Error indicator: contains an exception if the op. was not successful.
typedef boost::optional<std::exception> error_indicator;

/// Callback function for asynchronous operations without return value.
typedef std::function<void (error_indicator)> empty_cb;

/// Callback function for asynchronous operations with return values.
template <class ReturnType>
struct cb {
  /// Callback function used to pass returnvalues/errors asynchronously.
  typedef std::function<void (ReturnType, error_indicator)> type;
};

}  // namespace botscript_server

#endif  // CALLBACK_H_
