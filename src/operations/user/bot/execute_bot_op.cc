// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./execute_bot_op.h"

#include <memory>

#include "bot.h"

#include "../../../bs_server.h"
#include "../../../user.h"
#include "./bot_util.h"

namespace botscript_server {

execute_bot_op::execute_bot_op(const rapidjson::Document& doc)
    : user_op(doc),
      identifier_(doc["arguments"]["identifier"].GetString()),
      command_(doc["arguments"]["execute"]["command"].GetString()),
      argument_(doc["arguments"]["execute"]["argument"].GetString()) {
}

const std::string& execute_bot_op::identifier() const {
  return identifier_;
}

const std::string& execute_bot_op::command() const {
  return command_;
}

const std::string& execute_bot_op::argument() const {
  return argument_;
}

std::vector<std::string> execute_bot_op::type() const {
  return {"user", "bot", "execute"};
}

std::vector<msg_ptr> execute_bot_op::execute(bs_server& server,
                                             op_callback cb) const {
  user u = get_user_from_session(server);
  get_bot(u, server, identifier())->execute(command(), argument());
  return std::vector<msg_ptr>();
}

}  // namespace botscript_server
