// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "bs_server.h"

#include "bot.h"

namespace bs = botscript;

namespace botscript_server {

bs_server::bs_server(boost::asio::io_service* io_service,
                     std::shared_ptr<dust::key_value_store> store,
                     std::vector<std::string> packages,
                     sid_callback activity_cb,
                     session_end_cb session_end_callback)
    : io_service_(io_service),
      store_(std::move(store)),
      users_(store_, "users"),
      packages_(std::move(packages)),
      activity_cb_(std::move(activity_cb)),
      session_end_cb_(std::move(session_end_callback)) {
}

void bs_server::handle_connection_close(const std::string& sid) {
  // TODO(felix) implement
}

void bs_server::load_bots(std::function<void ()> on_finish) {
  // TODO(felix) implement
}

void bs_server::on_bot_load(
    std::shared_ptr<bs::bot> bot,
    std::string err,
    std::shared_ptr<std::vector<std::string>> configs_ptr) {
}

void bs_server::stop() {
  for (auto& bot : bots_) {
    bot.second->shutdown();
  }
  bots_.clear();
}

}  // namespace botscript_server
