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
#include "./bs_server_options.h"
#include "./ws_server_options.h"
#include "./dust_server_options.h"
#include "./botscript_server_version.h"

using namespace botscript_server;
using namespace dust;
using namespace dust_server;
using namespace botscript;

namespace po = boost::program_options;

int main(int argc, char** argv) {
  std::cout << "\n\tMakielskis Bot v" + version() << "\n\n";

  // Prepare configurations with default values.
  std::string file;
  ws_server_options wss_options("0.0.0.0", "9003");
  bs_server_options bss_options(true, false, "packages", true);
  dust_server_options ds_options("0.0.0.0", "9004", "");

  // Prepare program description.
  std::string config_file;
  po::options_description desc("\nOptions", 100);
  desc.add_options()
      ("help", "produce help message")
      ("config,c", po::value<std::string>(&file)->default_value("config.ini"),
          "name of a file for configuration.");
  wss_options.configure_description(desc);
  bss_options.configure_description(desc);
  ds_options.configure_description(desc);

  // Positional argument: config file.
  po::positional_options_description p;
  p.add("config", -1);

  // Read cmdline arguments.
  po::variables_map vm;
  auto parsed = po::command_line_parser(argc, argv)
                  .options(desc)
                    .positional(p)
                    .allow_unregistered()
                  .run();
  po::store(parsed, vm);
  po::notify(vm);

  // Print help and exit if desired.
  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 0;
  }

  // List unrecognized options.
  auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
  for (const auto& opt : unrecog) {
    std::cout << "Unrecognized option: " << opt << "\n";
  }

  // Read config file.
  std::ifstream ifs(file.c_str());
  if (!ifs) {
    std::cout << "Configuration file " << file << " not found, skipping\n";
  } else {
    po::store(po::parse_config_file(ifs, desc), vm);
    po::notify(vm);
  }

  // Read program options.
  wss_options.parse(vm);
  bss_options.parse(vm);
  ds_options.parse(vm);

  // Print options.
  std::cout << "Used options:\n" << wss_options << bss_options << ds_options
            << "\n";

  // Start servers.
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
