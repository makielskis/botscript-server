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
#include "./conf/options_parser.h"
#include "./conf/bs_server_options.h"
#include "./conf/ws_server_options.h"
#include "./conf/dust_server_options.h"

using namespace botscript_server;
using namespace dust;
using namespace dust_server;
using namespace botscript;

int main(int argc, char* argv[]) {
  std::cout << "\n\tMakielskis Bot v" + version() << "\n\n";

  ws_server_options wss_options("0.0.0.0", "9003");
  bs_server_options bss_options(true, false, "packages", true);
  dust_server_options ds_options("0.0.0.0", "9004", "");

  options_parser parser({ &wss_options, &bss_options, &ds_options });
  parser.read_command_line_args(argc, argv);

  if (parser.help()) {
    parser.print_help(std::cout);
    return 0;
  }

  parser.read_configuration_file();
  parser.print_unrecognized(std::cout);
  parser.print_used(std::cout);

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
