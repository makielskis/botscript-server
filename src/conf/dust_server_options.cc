// Copyright (c) 2014, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./dust_server_options.h"

#include "boost/program_options.hpp"

#define HOST_OPTION ("dust_server.host")
#define PORT_OPTION ("dust_server.port")
#define PASSWORD_OPTION ("dust_server.password")

namespace po = boost::program_options;

namespace botscript_server {

dust_server_options::dust_server_options(std::string host, std::string port,
                                         std::string password)
    : dust_server::options(std::move(host), std::move(port),
                           std::move(password)) {
}

po::options_description dust_server_options::desc() {
  boost::program_options::options_description desc("Database Server Options");
  desc.add_options()
    (HOST_OPTION, po::value<std::string>(&host_)->default_value(host_),
        "the IP the db script interface (e.g. 127.0.0.1 or 0.0.0.0)")
    (PORT_OPTION, po::value<std::string>(&port_)->default_value(port_),
        "the port for db interface (1-65536)")
    (PASSWORD_OPTION, po::value<std::string>(&password_),
        "admin password for db script interface");
  return desc;
}

std::ostream& operator<<(std::ostream& out,
                         const dust_server_options& options) {
  bool pw_set = !options.password_.empty();
  std::string pw_val = pw_set ? options.password_ : "!SECURITY WARNING! empty";

  return out << "  " << HOST_OPTION << ": " << options.host_ << "\n"
             << "  " << PORT_OPTION << ": " << options.port_ << "\n"
             << "  " << PASSWORD_OPTION << ": " << pw_val;
}

}  // namespace botscript_server
