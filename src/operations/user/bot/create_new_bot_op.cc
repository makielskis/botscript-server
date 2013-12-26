// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "create_new_bot_op.h"

#include "mem_bot_config.h"

#include "../../../bs_server.h"
#include "../../../user.h"
#include "../../../error.h"
#include "../../../db_bot_config.h"
#include "../../../error.h"

namespace botscript_server {

create_new_bot_op::create_new_bot_op(const std::string& sid,
                                     const std::string& config)
    : create_bot_op(sid),
      config_(config) {
}

create_new_bot_op::create_new_bot_op(const rapidjson::Document& doc)
    : create_bot_op(doc),
      config_(doc["arguments"]["config"].GetString()) {
}

const std::string& create_new_bot_op::config() const {
  return config_;
}

std::vector<std::string> create_new_bot_op::type() const {
  return {"user", "bot", "create", "new"};
}

std::shared_ptr<botscript::bot_config> create_new_bot_op::bot_config(
    const bs_server& server,
    const user& u) const {
  std::string identifier = bot_identifier();
  if (server.bots_.find(identifier) != server.bots_.end()) {
    throw boost::system::system_error(error::bot_already_exists);
  }
  auto config_doc = server.users_[u.username()]["bots"][identifier];
  return std::make_shared<db_bot_config>(config_doc, config());
}

void create_new_bot_op::on_load_fail(
    std::shared_ptr<botscript::bot_config> config,
    user u) const {
  u.remove_bot(config->identifier());
}

std::string create_new_bot_op::bot_identifier() const {
  botscript::mem_bot_config c;
  try {
    c = {config()};
  } catch(const std::runtime_error& e) {
    throw boost::system::system_error(error::invalid_configuration);
  }
  return c.identifier();
}

}  // namespace botscript_server

