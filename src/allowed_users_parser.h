#ifndef BOTSCRIPT_SERVER_ALLOWED_USERS_PARSER_H_
#define BOTSCRIPT_SERVER_ALLOWED_USERS_PARSER_H_

#include <string>
#include <vector>

#include "./bs_server_options.h"

namespace botscript_server {

#ifdef ANDROID

void check_user_allowed(bs_server_options const& opt, std::string const& name) {
}
void check_bot_allowed(std::istream& in, std::string const& name,
                       std::string const& bot) {
}
void check_user_allowed(std::istream& in, std::string const& name) {
}
void check_bot_allowed(bs_server_options const& opt, std::string const& name,
                       std::string const& bot) {
}

#else

void check_user_allowed(bs_server_options const& opt, std::string const& name);
void check_bot_allowed(std::istream& in, std::string const& name,
                       std::string const& bot);
void check_user_allowed(std::istream& in, std::string const& name);
void check_bot_allowed(bs_server_options const& opt, std::string const& name,
                       std::string const& bot);

#endif

}  // namespace botscript_server

#endif /* BOTSCRIPT_SERVER_ALLOWED_USERS_PARSER_H_ */
