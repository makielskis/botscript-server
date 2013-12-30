// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./delete_update_op.h"

#include "bot.h"
#include "bot_config.h"

#include "../../../user.h"
#include "../../../bs_server.h"
#include "../../../error.h"
#include "../../../make_unique.h"
#include "../../../messages/success_msg.h"

namespace botscript_server {

delete_update_op::delete_update_op(const rapidjson::Document& doc)
    : user_op(doc),
      current_pw_(doc["arguments"]["current_pw"].GetString()) {
}

const std::string& delete_update_op::current_pw() const {
  return current_pw_;
}

std::vector<std::string> delete_update_op::type() const {
  return {"user", "update", "delete"};
}

std::vector<msg_ptr> delete_update_op::execute(bs_server& server,
                                               op_callback /* cb */) const {
  user u = get_user_from_session(server);

  if (!u.check_password(current_pw())) {
    throw boost::system::system_error(error::password_wrong);
  }

  for (const auto& config : u.bot_configs()) {
    auto it = server.bots_.find(config->identifier());
    if (it != server.bots_.end()) {
      it->second->shutdown();
      server.bots_.erase(it);
    }
  }

  u.remove();

  std::vector<msg_ptr> out;
  out.emplace_back(make_unique<success_msg>(0, type()));
  return out;
}

}  // namespace botscript_server
