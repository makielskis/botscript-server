#include <iostream>
#include <thread>

#include <boost/asio/io_service.hpp>

#include "./botserver_handler.h"

using namespace websocketpp;
using namespace botscript_server;

int main() {
  short port = 9003;

  try {
    bot_server_handler* bsh = new bot_server_handler();
    server::handler::ptr handler(bsh);
    server endpoint(handler);
    bsh->io_service(endpoint.io_service());

    std::cout << "Starting (port: " << port << ") ...\n";
    endpoint.listen(port);
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
