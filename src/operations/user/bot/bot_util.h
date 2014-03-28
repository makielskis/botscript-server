// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_BOT_UTIL_H_
#define BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_BOT_UTIL_H_

#include <memory>
#include <string>
#include <map>

namespace botscript {
class bot;
}

namespace botscript_server {

class user;
class bs_server;

/// Checks whether the user u is owner of a bot with the given identifier.
///
/// \param u           the user to check
/// \param identifier  the identifier of the bot to check
void check_owner(const user& u, const std::string& identifier);

/// Extracts the bot from the server using the identifier.
/// If the bot is not owned by the user or does not exist at all, it throws an
/// bot_not_found error.
///
/// \param u           the user that pretends to own that bot
/// \param server      the server that runs the bot
/// \param identifier  the identifier of the bot
/// \return a shared pointer to the selected bot
std::shared_ptr<botscript::bot> get_bot(const user& u, const bs_server& server,
                                        const std::string& identifier);

/// Returns all bot configurations of active bots
/// (not in the bs_server::bot_creation_blocklist_!) owned by the user.
///
/// \param u       the user to get the sessions for
/// \param server  the server which is responsible
/// \return a map with the bot identifier as key and the bot config (without
///         password) as value
std::map<std::string, std::string> bot_configs(user const& u,
                                               bs_server& server);

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_BOT_UTIL_H_
