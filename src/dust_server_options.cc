// Copyright (c) 2014, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./dust_server_options.h"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

namespace botscript_server {

dust_server_options::dust_server_options(std::string host,
                                         std::string port,
                                         std::string password)
    : dust_server::options(std::move(host), std::move(port),
                           std::move(password)) {
}

dust_server_options::~dust_server_options() {
}

void dust_server_options::configure_description(
    boost::program_options::options_description& desc) {
  desc.add_options()
    ("dust_server.host", po::value<std::string>()->default_value(host_),
        "the IP the db script interface (e.g. 127.0.0.1 or 0.0.0.0)")
    ("dust_server.port", po::value<std::string>()->default_value(port_),
        "the port for db interface (1-65536)")
    ("dust_server.password", po::value<std::string>(),
        "admin password for db script interface");
}

void dust_server_options::parse(
    const boost::program_options::variables_map& vm) {
  host_ = vm["dust_server.host"].as<std::string>();
  port_ = vm["dust_server.port"].as<std::string>();

  if (vm.count("dust_server.password")) {
    password_ = vm["dust_server.password"].as<std::string>();
  }
}

std::ostream& operator<<(std::ostream& out,
                         const dust_server_options& options) {
  std::string pw_val = options.password_;
  if (pw_val.empty()) {
    pw_val = "!SECURITY WARNING! empty";
  }

  out << "\n" << "  dust_server.host: " << options.host_ << "\n"
  << "  dust_server.port: " << options.port_ << "\n"
  << "  dust_server.password: " << pw_val << "\n";
  return out;
}

}  // namespace botscript_server
