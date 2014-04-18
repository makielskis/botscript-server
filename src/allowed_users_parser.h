// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_ALLOWED_USERS_PARSER_H_
#define BOTSCRIPT_SERVER_ALLOWED_USERS_PARSER_H_

#include <string>
#include <vector>

namespace botscript_server {

class bs_server_options;

void check_user_allowed(bs_server_options const& opt, std::string const& name);
void check_bot_allowed(std::istream& in, std::string const& name,
                       std::string const& bot);
void check_user_allowed(std::istream& in, std::string const& name);
void check_bot_allowed(bs_server_options const& opt, std::string const& name,
                       std::string const& bot);

}  // namespace botscript_server

#endif /* BOTSCRIPT_SERVER_ALLOWED_USERS_PARSER_H_ */
