// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "service.h"

#include <boost/asio/io_service.hpp>

#include "./storage/file_config_store.h"
#include "./botserver_handler.h"

namespace botscript_server {

using namespace websocketpp;

service::service()
  : io_service_(nullptr) {
}

void service::start(const std::string& host, const std::string& service) {
  try {
    // Instatiate handler and Web Socket server endpoint.
    file_config_store store("./configs", nullptr);
    bot_server_handler* bsh = new bot_server_handler(store);
    server::handler::ptr handler(bsh);
    server endpoint(handler);
    store.io_service(endpoint.io_service());
    bsh->io_service(endpoint.io_service());

    io_service_ = endpoint.io_service();
    endpoint.listen(host, service);
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}

void service::stop() {
  io_service_->stop();
}

}  // namespace botscript_server
