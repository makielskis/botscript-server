// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./reactivate_bot_op.h"

#include "../../../bs_server.h"
#include "../../../error.h"
#include "../../../user.h"
#include "bot_config.h"

namespace botscript_server {

reactivate_bot_op::reactivate_bot_op(const rapidjson::Document& doc)
    : create_bot_op(doc),
      identifier_(doc["arguments"]["identifier"].GetString()),
      proxy_(doc["arguments"]["proxy"].GetString()) {
}

const std::string& reactivate_bot_op::identifier() const {
  return identifier_;
}

const std::string& reactivate_bot_op::proxy() const {
  return proxy_;
}

std::shared_ptr<botscript::bot_config> reactivate_bot_op::bot_config(
    const bs_server& server,
    const user& u) const {
  auto bot_config = u.bot_config(identifier());

  if (!bot_config->inactive()) {
    throw boost::system::system_error(error::bot_not_inactive);
  }

  if (server.bots_.find(bot_config->identifier()) != server.bots_.end()) {
    throw boost::system::system_error(error::bot_already_exists);
  }

  bot_config->set("base", "proxy", proxy());

  return bot_config;
}

void reactivate_bot_op::on_load_fail(
    std::shared_ptr<botscript::bot_config> config,
    user u) const {
}

std::vector<std::string> reactivate_bot_op::type() const {
  return {"user", "bot", "create", "reactivate"};
}

}  // namespace botscript_server
