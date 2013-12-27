// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./create_bot_op.h"

#include "boost/bind.hpp"

#include "bot.h"
#include "bot_config.h"

#include "../../../bs_server.h"
#include "../../../error.h"
#include "../../../user.h"
#include "../../../make_unique.h"
#include "../../../messages/bots_msg.h"
#include "../../../messages/failure_msg.h"

namespace botscript_server {

create_bot_op::create_bot_op(const std::string& sid)
    : user_op(sid) {
}

create_bot_op::create_bot_op(const rapidjson::Document& doc)
    : user_op(doc) {
}

create_bot_op::~create_bot_op() {
}

std::vector<msg_ptr> create_bot_op::execute(bs_server& server,
                                            op_callback cb) const {
  user u = get_user_from_session(server);

  auto self = shared_from_this();
  auto bot = std::make_shared<botscript::bot>(server.io_service_);
  bot->update_callback_ = server.sid_cb(sid());
  bot->init(bot_config(server, u), [cb, u, &server, self](
      std::shared_ptr<botscript::bot> bot,
      std::string err) {
    std::string identifier = bot->configuration().identifier();

    bool success = err.empty();
    bot->configuration().inactive(!success);

    std::vector<msg_ptr> out;
    if (success) {
      server.bots_[bot->configuration().identifier()] = bot;
      out.emplace_back(make_unique<bots_msg>(self->bot_configs(u)));
    } else {
      self->on_load_fail(bot->config(), u);
      bot->shutdown();
      std::vector<std::string> type = {"user", "bot", "create"};
      boost::system::error_code ec = error::bot_creation_failed;
      std::string message = ec.message() + ": " + err;
      out.emplace_back(make_unique<failure_msg>(0, type, ec.value(), message));
    }

    cb("", std::move(out));
  });

  return {};
}

}  // namespace botscript_server
