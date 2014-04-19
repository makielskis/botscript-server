// Copyright (c) 2014, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./ws_server_options.h"

#include "boost/program_options.hpp"

#define HOST_OPTION ("websocket_server.host")
#define PORT_OPTION ("websocket_server.port")

namespace po = boost::program_options;

namespace botscript_server {

ws_server_options::ws_server_options(std::string host_default,
                                     std::string port_default)
    : host(std::move(host_default)),
      port(std::move(port_default)) {
}

po::options_description ws_server_options::desc() {
  po::options_description desc("Websocket Server Options");
  desc.add_options()
    (HOST_OPTION, po::value<std::string>(&host)->default_value(host),
        "bot server websocket host (e.g. 127.0.0.1 or 0.0.0.0)")
    (PORT_OPTION, po::value<std::string>(&port)->default_value(port),
        "bot server websocket port (1-65536)");
  return desc;
}

void ws_server_options::print(std::ostream& out) const {
  out << "  " << HOST_OPTION << ": " << host << "\n"
      << "  " << PORT_OPTION << ": " << port;
}

std::ostream& operator<<(std::ostream& out, const ws_server_options& options) {
  options.print(out);
  return out;
}

}  // namespace botscript_server

