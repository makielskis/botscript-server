#include "./allowed_users_parser.h"


#ifdef ANDROID
namespace botscript_server {

void check_user_allowed(bs_server_options const&, std::string const&) {
}
void check_bot_allowed(std::istream&, std::string const&,
                       std::string const&) {
}
void check_user_allowed(std::istream&, std::string const&) {
}
void check_bot_allowed(bs_server_options const&, std::string const&,
                       std::string const&) {
}

}  // namespace botscript_server
#else

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"

#include "./error.h"

namespace botscript_server {

struct allowed_user {
  std::string name;
  std::vector<std::string> allowed_bots;
  bool all_bots_allowed;

  template<class Archive>
  void serialize(Archive & ar) {
    ar(CEREAL_NVP(name));
    ar(CEREAL_NVP(allowed_bots));
    ar(CEREAL_NVP(all_bots_allowed));
  }
};

std::vector<allowed_user> read_allowed_users(std::istream& in) {
  std::vector<allowed_user> allowed_users;
  try {
    cereal::JSONInputArchive archive(in);
    archive(allowed_users);
  } catch (const cereal::Exception& e) {
    throw boost::system::system_error(error::allowed_users_read_error);
  }
  return allowed_users;
}

void check_user_allowed(std::istream& in, std::string const& name) {
  std::vector<allowed_user> allowed_users = read_allowed_users(in);

  auto it = std::find_if(std::begin(allowed_users), std::end(allowed_users),
                         [&name](allowed_user const& allowed_user) {
                           return allowed_user.name == name;
                         });

  if (it == std::end(allowed_users)) {
    throw boost::system::system_error(error::user_not_allowed);
  }
}

void check_user_allowed(bs_server_options const& opt, std::string const& name) {
  if (opt.allowed_users().empty()) {
    return;
  }

  std::ifstream f;
  f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    f.open(opt.allowed_users());
    check_user_allowed(f, name);
    f.close();
  } catch (const std::ifstream::failure&) {
    throw boost::system::system_error(error::allowed_users_read_error);
  }
}

void check_bot_allowed(std::istream& in, std::string const& name,
                       std::string const& bot) {
  std::vector<allowed_user> allowed_users = read_allowed_users(in);

  // Check if user is allowed (should be!).
  auto user_it = std::find_if(std::begin(allowed_users),
                              std::end(allowed_users),
                              [&name](allowed_user const& allowed_user) {
                                return allowed_user.name == name;
                              });
  if (user_it == std::end(allowed_users)) {
    throw boost::system::system_error(error::user_not_allowed);
  }

  // Check if user is free to create any bot.
  if (user_it->all_bots_allowed) {
    return;
  }

  // Check if bot is allowed.
  auto bot_it = std::find_if(std::begin(user_it->allowed_bots),
                             std::end(user_it->allowed_bots),
                             [&bot](std::string const& allowed_bot) {
                               return allowed_bot == bot;
                             });
  if (bot_it == std::end(user_it->allowed_bots)) {
    throw boost::system::system_error(error::bot_not_allowed);
  }
}

void check_bot_allowed(bs_server_options const& opt, std::string const& name,
                       std::string const& bot) {
  if (opt.allowed_users().empty()) {
    return;
  }

  std::ifstream f;
  f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    f.open(opt.allowed_users());
    check_bot_allowed(f, name, bot);
    f.close();
  } catch (const std::ifstream::failure&) {
    throw boost::system::system_error(error::allowed_users_read_error);
  }
}

}  // namespace botscript_server
#endif
