// Copyright (c) 2014, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./bs_server_options.h"

#include "boost/program_options.hpp"
#include "boost/io/ios_state.hpp"

#define FORCEPROXY_OPTION "botscript_server.forceproxy"
#define AUTOLOGIN_OPTION "botscript_server.autologin"
#define PACKAGES_PATH_OPTION "botscript_server.packages_path"
#define BOTLOG_OPTION "botscript_server.botlog"
#define ALLOWED_USERS_OPTION "botscript_server.allowed_users"

namespace po = boost::program_options;

namespace botscript_server {

bs_server_options::bs_server_options(bool forceproxy_default,
                                     bool autologin_default,
                                     std::string default_packages_path,
                                     bool botlog_default)
    : forceproxy(forceproxy_default),
      autologin(autologin_default),
      packages_path(default_packages_path),
      botlog(botlog_default) {
}

po::options_description bs_server_options::desc() {
  po::options_description desc("Botscript Server Options");
  desc.add_options()
    (FORCEPROXY_OPTION, po::value<bool>(&forceproxy)->default_value(forceproxy),
        "forces the user to set a bot proxy")
    (AUTOLOGIN_OPTION, po::value<bool>(&autologin)->default_value(autologin),
        "single user mode with autologin (no authentification needed)")
    (PACKAGES_PATH_OPTION, po::value<std::string>(&packages_path)->default_value(packages_path),
        "directory where the botscript packages are located")
    (BOTLOG_OPTION, po::value<bool>(&botlog)->default_value(botlog),
        "whether the bots should print their output to stdout")
    (ALLOWED_USERS_OPTION, po::value<std::string>(&allowed_users)->default_value(allowed_users),
        "file containing authorized users (see manual for file format)");
  return desc;
}

std::ostream& operator<<(std::ostream& out, const bs_server_options& options) {
  boost::io::ios_all_saver guard(out);
  return out << "  " << FORCEPROXY_OPTION << ": " << std::boolalpha
             << options.forceproxy << "\n"
             << "  " << AUTOLOGIN_OPTION << ": " << std::boolalpha
             << options.autologin << "\n"
             << "  " << PACKAGES_PATH_OPTION
             << ": " << options.packages_path << "\n"
             << "  " << BOTLOG_OPTION << ": " << options.botlog << "\n"
             << "  " << ALLOWED_USERS_OPTION << ": " << options.allowed_users;
}

}  // namespace botscript_server
