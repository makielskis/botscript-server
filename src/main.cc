// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include <iostream>
#include <iostream>
#include <fstream>
#include <memory>
#include <cstdlib>

#include "boost/program_options.hpp"
#include "boost/asio/io_service.hpp"

#include "dust/storage/cached_db.h"

#include "dust-server/http_service.h"

#include "./ws_server.h"
#include "./botscript_server_version.h"
#include "./conf/bs_server_options.h"
#include "./conf/ws_server_options.h"
#include "./conf/dust_server_options.h"

using namespace botscript_server;
using namespace dust;
using namespace dust_server;
using namespace botscript;

namespace po = boost::program_options;

class settings {
public:
  settings(std::vector<configuration*> options)
      : desc_("General"),
        options_(std::move(options)) {
    configure_description();
  }

  void configure_description() {
    std::string f;

    desc_.add_options()
        ("help", "produce help message")
        ("config,c", po::value<std::string>(&f)->default_value("config.ini"),
            "config path");

    for (auto& option : options_) {
      desc_.add(option->desc());
    }
  }

  void read_command_line_args(int argc, char* argv[]) {
    po::positional_options_description p;
    p.add("config", -1);

    auto parsed = po::command_line_parser(argc, argv)
                    .options(desc_)
                      .positional(p)
                      .allow_unregistered()
                    .run();
    po::store(parsed, vm_);
    po::notify(vm_);

    auto unrecog_cmdl = po::collect_unrecognized(parsed.options,
                                                 po::include_positional);
    unrecog_.insert(unrecog_.end(), unrecog_cmdl.begin(), unrecog_cmdl.end());
  }

  void read_configuration_file() {
    std::ifstream ifs(file_.c_str());
    if (ifs) {
      auto parsed_file_options = po::parse_config_file(ifs, desc_, true);

      // Add unrecognized file options to the global unrecognized options.
      auto unrecog_file = po::collect_unrecognized(parsed_file_options.options,
                                                   po::include_positional);
      unrecog_.insert(unrecog_.end(), unrecog_file.begin(), unrecog_file.end());

      po::store(parsed_file_options, vm_);
      po::notify(vm_);
    }
  }

  bool quit() {
    if (vm_.count("help")) {
      std::cout << desc_ << "\n";
      return true;
    }

    return false;
  }

  void print_unrecognized(std::ostream& out) {
    out << "Unrecognized options:\n";
    for (auto& unrecog_option : unrecog_) {
      out << "  " << unrecog_option << "\n";
    }
    out << "\n";
  }

private:
  std::string file_;
  po::options_description desc_;
  std::vector<std::string> unrecog_;
  po::variables_map vm_;
  std::vector<configuration*> options_;
};

int main(int argc, char* argv[]) {
  std::cout << "\n\tMakielskis Bot v" + version() << "\n\n";

  ws_server_options wss_options("0.0.0.0", "9003");
  bs_server_options bss_options(true, false, "packages", true);
  dust_server_options ds_options("0.0.0.0", "9004", "");

  settings options({ &wss_options, &bss_options, &ds_options });
  options.read_command_line_args(argc, argv);

  if (options.quit()) {
    return 0;
  }

  options.read_configuration_file();
  options.print_unrecognized(std::cout);

  std::cout << "Used Options: " << "\n\n";
  std::cout << wss_options << "\n\n";
  std::cout << bss_options << "\n\n";
  std::cout << ds_options << "\n\n";

  auto store = std::make_shared<cached_db>("db");
  boost::asio::io_service ios;
  ws_server s(std::move(wss_options), std::move(bss_options), &ios, store);
  http_service db_service(&ios, store, std::move(ds_options));
  s.start();

  std::function<void()> run = [&ios, &run]() {
    try {
      ios.run();
    } catch (const websocketpp::lib::error_code& ec) {
      std::cerr << "WARNING: Ignoring websocketpp error!\n";
      run();
    }
  };
  run();

  std::cout << "... exit\n";
}
