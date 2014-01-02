// Copyright (c) 2014, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./ws_server_options.h"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

namespace botscript_server {

ws_server_options::ws_server_options(std::string host_default,
                                     std::string port_default)
    : host_(std::move(host_default)),
      port_(std::move(port_default)) {
}

void ws_server_options::configure_description(
    boost::program_options::options_description& desc) {
  desc.add_options()
    ("websocket_server.host", po::value<std::string>()->default_value(host_),
        "bot server websocket host (e.g. 127.0.0.1 or 0.0.0.0)")
    ("websocket_server.port", po::value<std::string>()->default_value(port_),
        "bot server websocket port (1-65536)");
}

void ws_server_options::parse(const po::variables_map& vm) {
  host_ = vm["websocket_server.host"].as<std::string>();
  port_ = vm["websocket_server.port"].as<std::string>();
}

std::string ws_server_options::host() const {
  return host_;
}

std::string ws_server_options::port() const {
  return port_;
}

std::ostream& operator<<(std::ostream& out, const ws_server_options& options) {
  out << "\n  websocket_server.host: " << options.host() << "\n"
      << "  websocket_server.port: " << options.port() << "\n";
  return out;
}

}  // namespace botscript_server

