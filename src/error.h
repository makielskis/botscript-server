// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSERVER_ERROR_H_
#define BOTSERVER_ERROR_H_

#include <iostream>

#include "boost/system/system_error.hpp"
#include "boost/type_traits.hpp"

namespace botscript_server {

namespace error {
  // Specific errors
  enum error_code_t {
    // incoming message invalid
    invalid_message = 1,

    // registration errors
    username_already_taken = 11,
    password_too_short = 12,
    email_address_not_valid = 13,

    // user error
    user_not_found = 21,

    // session errors
    session_id_not_available = 31,
    session_id_timed_out = 32,

    // password
    password_wrong = 41,

    // bot
    bot_already_exists = 51,
    bot_not_found = 52,
    invalid_configuration = 53,
    bot_creation_failed = 54,
    bot_in_blocklist = 55,
    bot_not_inactive = 56,
    proxy_required = 57
  };

  // error_condition
  enum error_condition_t {
    // session
    session_error = 1
  };

  boost::system::error_code make_error_code(error_code_t e) noexcept;
  boost::system::error_condition make_error_condition(error_condition_t e) noexcept;
}

class error_category_impl : public boost::system::error_category {
 public:
  virtual const char* name() const noexcept;
  virtual std::string message(int ev) const noexcept;
  virtual boost::system::error_condition default_error_condition(int ev) const noexcept;
};

const boost::system::error_category& error_category();

}  // namespace botscript_server

namespace boost {
namespace system {
  template <>
  struct is_error_code_enum<botscript_server::error::error_code_t>
    : public boost::true_type {};

  template <>
  struct is_error_condition_enum<botscript_server::error::error_condition_t>
    : public boost::true_type {};
}  // namespace system
}  // namespace boost

#endif  // BOTSERVER_ERROR_H_
