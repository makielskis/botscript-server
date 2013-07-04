#include "service.h"

#include <boost/asio/io_service.hpp>

#include "./botserver_handler.h"

namespace botscript_server {

using namespace websocketpp;

service::service()
  : io_service_(nullptr) {
}

void service::start(short ws_port) {
  try {
    // Instatiate handler and Web Socket server endpoint.
    bot_server_handler* bsh = new bot_server_handler();
    server::handler::ptr handler(bsh);
    server endpoint(handler);
    bsh->io_service(endpoint.io_service());

    io_service_ = endpoint.io_service();
    endpoint.listen(ws_port);
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}

void service::stop() {
  io_service_->stop();
}

}  // namespace botscript_server
