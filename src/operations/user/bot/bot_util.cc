#include "bot_util.h"

#include "bot_config.h"

#include "../../../bs_server.h"
#include "../../../user.h"
#include "../../../error.h"

namespace botscript_server {

std::shared_ptr<botscript::bot> get_bot(const user& u, const bs_server& server,
                                        const std::string& identifier) {
  if (!u.has_bot(identifier)) {
    throw boost::system::system_error(error::bot_not_found);
  }

  auto bot = server.bots_.find(identifier);
  if (bot == server.bots_.end()) {
    throw boost::system::system_error(error::bot_not_found);
  }

  return bot->second;
}

std::map<std::string, std::string> bot_configs(user const& u,
                                               bs_server& server) {
  std::map<std::string, std::string> bots;
  auto const& blocklist = server.bot_creation_blocklist_;
  for (auto const& config : u.bot_configs()) {
    if (blocklist.find(config->identifier()) == blocklist.end()) {
      bots[config->identifier()] = config->to_json(false);
    }
  }
  return bots;
}

}  // namespace botscript_server
