// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef CALLBACK_H_
#define CALLBACK_H_

#include "boost/system/error_code.hpp"

namespace botscript_server {

/// Callback function for asynchronous operations without return value.
typedef std::function<void (boost::system::error_code ec)> empty_cb;

/// Callback function for asynchronous operations with return values.
template <class ReturnType>
struct cb {
  /// Callback function used to pass returnvalues/errors asynchronously.
  typedef std::function<void (ReturnType, boost::system::error_code)> type;
};

}  // namespace botscript_server

#endif  // CALLBACK_H_
