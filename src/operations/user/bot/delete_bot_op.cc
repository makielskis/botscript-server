// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./delete_bot_op.h"

#include "bot.h"

#include "../../../bs_server.h"
#include "../../../error.h"
#include "../../../make_unique.h"
#include "../../../messages/bots_msg.h"
#include "../../../user.h"
#include "./bot_util.h"

namespace botscript_server {

delete_bot_op::delete_bot_op(const std::string& sid,
                             const std::string& identifier)
    : user_op(sid), identifier_(identifier) {}

delete_bot_op::delete_bot_op(const rapidjson::Document& doc)
    : user_op(doc), identifier_(doc["arguments"]["identifier"].GetString()) {}

const std::string& delete_bot_op::identifier() const { return identifier_; }

std::vector<std::string> delete_bot_op::type() const {
  return {"user", "bot", "delete"};
}

std::vector<msg_ptr> delete_bot_op::execute(bs_server& server,
                                            op_callback /* cb */) const {
  // Check whether user ownes the specified bot.
  user u = get_user_from_session(server);
  if (!u.has_bot(identifier())) {
    throw boost::system::system_error(error::bot_not_found);
  }

  // Don't delete bots in the blocklist.
  const auto& blocklist = server.bot_creation_blocklist_;
  if (blocklist.find(identifier()) != blocklist.end()) {
    throw boost::system::system_error(error::bot_in_blocklist);
  }

  // Stop bot (don't care if it does not exist
  // then it's probably inactive).
  try {
    get_bot(u, server, identifier())->shutdown();
    server.bots_.erase(identifier());
  } catch (const boost::system::system_error&) {
  }

  // Remove bot configuration from the database.
  u.remove_bot(identifier());

  std::vector<msg_ptr> out;
  out.emplace_back(make_unique<bots_msg>(bot_configs(u, server)));
  return out;
}

}  // namespace botscript_server
