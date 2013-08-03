// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./error.h"

namespace botscript_server {

namespace error {
  boost::system::error_code make_error_code(error_code_t e) noexcept {
    return boost::system::error_code(static_cast<int>(e), error_category());
  }

  boost::system::error_condition make_error_condition(error_condition_t e) noexcept {
    return boost::system::error_condition(static_cast<int>(e), error_category());
  }
}  // namespace error

const char* error_category_impl::name() const noexcept {
    return "botserver";
}

std::string error_category_impl::message(int ev) const noexcept {
  switch (ev) {
    case error::username_already_taken:
      return "Username already in use.";

    case error::password_too_short:
      return "Password too short";

    case error::email_address_not_valid:
      return "Email address not valid";

    case error::session_id_not_available:
      return "Session invalid";

    case error::session_id_timed_out:
      return "Session timeout";

    case error::password_wrong:
      return "Password wrong";

    case error::bot_already_exists:
      return "Bot already exists";

    case error::bot_not_found:
      return "Bot not found";

    default:
      return "Unknown error";
  }
}

boost::system::error_condition
    error_category_impl::default_error_condition(int ev) const noexcept {
  switch (ev)
  {
    case error::session_id_not_available:
    case error::session_id_timed_out:
      return boost::system::error_condition(error::session_error, *this);

    default:
      return boost::system::error_condition(ev, *this);
  }
}

const boost::system::error_category& error_category() {
  static error_category_impl instance;
  return instance;
}

}  // namespace botscript_server
