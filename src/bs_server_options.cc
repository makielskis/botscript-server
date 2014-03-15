// Copyright (c) 2014, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./bs_server_options.h"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

namespace botscript_server {

bs_server_options::bs_server_options(bool forceproxy_default,
                                     bool autologin_default,
                                     std::string default_packages_path,
                                     bool botlog)
    : forceproxy_(forceproxy_default),
      autologin_(autologin_default),
      packages_path_(default_packages_path),
      botlog_(botlog) {
}

void bs_server_options::configure_description(po::options_description& desc) {
  desc.add_options()
    ("botscript_server.forceproxy", po::value<bool>()->default_value(forceproxy_),
        "forces the user to set a bot proxy")
    ("botscript_server.autologin", po::value<bool>()->default_value(autologin_),
        "single user mode with autologin (no authentification needed)")
    ("botscript_server.packages_path", po::value<std::string>()->default_value(packages_path_),
        "directory where the botscript packages are located")
    ("botscript_server.botlog", po::value<bool>()->default_value(botlog_),
        "whether the bots should print their output to stdout")
    ("botscript_server.allowed_users", po::value<std::string>()->default_value(allowed_users_),
        "path to allowed_users.txt (one user per line), leave empty to disable checking");
}

void bs_server_options::parse(const po::variables_map& vm) {
  forceproxy_ = vm["botscript_server.forceproxy"].as<bool>();
  autologin_ = vm["botscript_server.autologin"].as<bool>();
  packages_path_ = vm["botscript_server.packages_path"].as<std::string>();
  botlog_ = vm["botscript_server.botlog"].as<bool>();
  allowed_users_ = vm["botscript_server.allowed_users"].as<std::string>();
}

bool bs_server_options::forceproxy() const {
  return forceproxy_;
}

bool bs_server_options::autologin() const {
  return autologin_;
}

std::string bs_server_options::packages_path() const {
  return packages_path_;
}

bool bs_server_options::botlog() const {
  return botlog_;
}

std::string bs_server_options::allowed_users() const {
  return allowed_users_;
}

std::ostream& operator<<(std::ostream& out, const bs_server_options& options) {
  out << "\n  botscript_server.forceproxy: "
      << std::boolalpha << options.forceproxy() << "\n"
      << "  botscript_server.autologin: "
      << std::boolalpha << options.autologin() << "\n"
      << "  botscript_server.packages_path: " << options.packages_path() << "\n"
      << "  botscript_server.botlog: " << options.botlog() << "\n"
      << "  botscript_server.allowed_users: " << options.allowed_users()
      << "\n";
  return out;
}

}  // namespace botscript_server
