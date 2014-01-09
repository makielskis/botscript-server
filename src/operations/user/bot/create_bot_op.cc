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

struct blocklist_element {
 public:
  blocklist_element(bs_server& server, user u, std::string identifier)
      : server_(server),
        u_(std::move(u)),
        identifier_(std::move(identifier)),
        freed_(false) {
    auto& user_blocklist = server.user_bot_creation_blocklist_;
    if (user_blocklist.find(u_.username()) != user_blocklist.end()) {
      throw boost::system::system_error(error::user_in_blocklist);
    }
    user_blocklist.insert(u_.username());

    auto& bot_blocklist = server.bot_creation_blocklist_;
    if (bot_blocklist.find(identifier_) != bot_blocklist.end()) {
      throw boost::system::system_error(error::bot_in_blocklist);
    }
    bot_blocklist.insert(identifier_);
  }

  blocklist_element(const blocklist_element&) = delete;
  blocklist_element& operator=(const blocklist_element&) = delete;

  virtual ~blocklist_element() {
      free_block_list();
  }

  void free_block_list() {
    if (!freed_) {
      freed_ = true;
      server_.user_bot_creation_blocklist_.erase(u_.username());
      server_.bot_creation_blocklist_.erase(identifier_);
    }
  }

  bs_server& server_;
  user u_;
  std::string identifier_;
  bool freed_;
};

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

  auto block = std::make_shared<blocklist_element>(server, u, bot_id());

  auto config = bot_config(server, u);
  if (server.options_.forceproxy() && config->value_of("base_proxy").empty()) {
    throw boost::system::system_error(error::proxy_required);
  }

  auto self = shared_from_this();
  auto bot = std::make_shared<botscript::bot>(server.io_service_);
  bot->update_callback_ = server.sid_cb(sid());
  bot->init(config, [cb, u, &server, self, block](
      std::shared_ptr<botscript::bot> bot,
      std::string err) {
    try {
      std::vector<msg_ptr> out;
      std::string identifier = bot->config()->identifier();

      bool success = err.empty();
      bot->config()->inactive(!success);

      if (success) {
        block->free_block_list();
        server.bots_[bot->config()->identifier()] = bot;
        out.emplace_back(make_unique<bots_msg>(self->bot_configs(u, server)));
      } else {
        self->on_load_fail(bot->config(), u);
        bot->shutdown();
        boost::system::error_code ec = error::bot_creation_failed;
        std::string message = ec.message() + ": " + err;
        out.emplace_back(make_unique<failure_msg>(0, self->type(), ec.value(),
                                                  message));
      }

      return cb("", std::move(out));
    } catch (const boost::system::system_error& e) {
      std::vector<msg_ptr> out;
      out.emplace_back(make_unique<failure_msg>(0, self->type(),
                                                e.code().value(),
                                                e.code().message()));
      return cb("", std::move(out));
    } catch (const std::exception& e) {
      std::vector<msg_ptr> out;
      boost::system::error_code ec = error::unknown_error;
      std::string message = ec.message() + e.what();
      out.emplace_back(make_unique<failure_msg>(0, self->type(), ec.value(),
                                                message));
      return cb("", std::move(out));
    } catch (...) {
      std::vector<msg_ptr> out;
      boost::system::error_code ec = error::unknown_error;
      out.emplace_back(make_unique<failure_msg>(0, self->type(), ec.value(),
                                                ec.message()));
      return cb("", std::move(out));
    }
  });

  return std::vector<msg_ptr>();
}

}  // namespace botscript_server
