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
  blocklist_element(bs_server& server, user u)
      : server_(server),
        u_(std::move(u)) {
    auto& blocklist = server.user_bot_creation_blocklist_;
    if (blocklist.find(u_.username()) != blocklist.end()) {
      throw boost::system::system_error(error::user_in_blocklist);
    }
    blocklist.insert(u_.username());
  }

  blocklist_element(const blocklist_element&) = delete;
  blocklist_element& operator=(const blocklist_element&) = delete;

  virtual ~blocklist_element() {
    server_.user_bot_creation_blocklist_.erase(u_.username());
  }

  bs_server& server_;
  user u_;
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

  auto blocker = std::make_shared<blocklist_element>(server, u);

  auto config = bot_config(server, u);
  if (server.force_proxy_ && config->value_of("base_proxy").empty()) {
    throw boost::system::system_error(error::proxy_required);
  }

  auto self = shared_from_this();
  auto bot = std::make_shared<botscript::bot>(server.io_service_);
  bot->update_callback_ = server.sid_cb(sid());
  bot->init(config, [cb, u, &server, self, blocker](
      std::shared_ptr<botscript::bot> bot,
      std::string err) {
    try {
      std::vector<msg_ptr> out;
      std::string identifier = bot->configuration().identifier();

      bool success = err.empty();
      bot->configuration().inactive(!success);

      if (success) {
        server.bots_[bot->configuration().identifier()] = bot;
        out.emplace_back(make_unique<bots_msg>(self->bot_configs(u)));
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
