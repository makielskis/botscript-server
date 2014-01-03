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
        "directory where the botscript packages are located");
}

void bs_server_options::parse(const po::variables_map& vm) {
  forceproxy_ = vm["botscript_server.forceproxy"].as<bool>();
  autologin_ = vm["botscript_server.autologin"].as<bool>();
  packages_path_ = vm["botscript_server.packages_path"].as<std::string>();
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

std::ostream& operator<<(std::ostream& out, const bs_server_options& options) {
  out << "\n  botscript_server.forceproxy: "
      << std::boolalpha << options.forceproxy() << "\n"
      << "  botscript_server.autologin: "
      << std::boolalpha << options.autologin() << "\n"
      << "  botscript_server.packages_path: " << options.packages_path() << "\n"
      << "  botscript_server.botlog: " << options.botlog() << "\n"
      << "\n";
  return out;
}

}  // namespace botscript_server
