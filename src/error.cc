// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./error.h"

namespace botscript_server {

namespace error {
  boost::system::error_code make_error_code(error_code_t e) BSS_NOEXCEPT {
    return boost::system::error_code(static_cast<int>(e), error_category());
  }

  boost::system::error_condition make_error_condition(error_condition_t e) BSS_NOEXCEPT {
    return boost::system::error_condition(static_cast<int>(e), error_category());
  }
}  // namespace error

const char* error_category_impl::name() const BSS_NOEXCEPT {
    return "botserver";
}

std::string error_category_impl::message(int ev) const BSS_NOEXCEPT {
  switch (ev) {
    case error::invalid_message:
      return "Invalid message.";

    case error::username_already_taken:
      return "Username already in use.";

    case error::password_too_short:
      return "Password too short";

    case error::email_address_not_valid:
      return "Email address not valid";

    case error::user_not_allowed:
      return "You are not allowed to register";

    case error::allowed_users_read_error:
      return "Could not read allowed users";

    case error::user_not_found:
      return "User not found";

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

    case error::invalid_configuration:
      return "Configuration not valid";

    case error::bot_creation_failed:
      return "Creating bot failed";

    case error::bot_not_inactive:
      return "Can't restore bot, it's not inactive";

    case error::bot_in_blocklist:
      return "Can't create/delete bot while bot creation";

    case error::proxy_required:
      return "You need to provide a proxy";

    case error::user_in_blocklist:
      return "You can only create one bot at the time";

    case error::unknown_error:
    default:
      return "Unknown error";
  }
}

boost::system::error_condition
error_category_impl::default_error_condition(int ev) const BSS_NOEXCEPT {
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
