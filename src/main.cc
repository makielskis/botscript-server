// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include <iostream>
#include <iostream>
#include <fstream>
#include <memory>

#include "boost/program_options.hpp"
#include "boost/asio/io_service.hpp"

#include "dust/storage/cached_db.h"

#include "dust-server/http_service.h"

#include "bot.h"

#include "./ws_server.h"

using namespace botscript_server;
using namespace dust;
using namespace dust_server;
using namespace botscript;

namespace po = boost::program_options;

int main(int argc, char** argv) {
  std::string config_file;

  // Prepare program description.
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("bot_ip",   po::value<std::string>()->default_value("0.0.0.0"),
          "the IP address websocket (e.g. 127.0.0.1 or 0.0.0.0)")
      ("bot_port", po::value<unsigned short>()->default_value(9003),
          "the port for websocket (1-65536)")
      ("db_ip",   po::value<std::string>()->default_value("0.0.0.0"),
          "the IP the db script interface (e.g. 127.0.0.1 or 0.0.0.0)")
      ("db_port", po::value<unsigned short>()->default_value(9004),
          "the port for db interface (1-65536)")
      ("db_password", po::value<std::string>(),
          "admin password for db script interface (e.g. 127.0.0.1 or 0.0.0.0)")
      ("config", po::value<std::string>(&config_file)
                     ->default_value("botscript-server.cfg"),
          "name of a file for configuration.");


  // Read cmdline arguments.
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);


  // Print help and exit if desired.
  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 0;
  }

  // Read config file.
  std::ifstream ifs(config_file.c_str());
  if (!ifs) {
    std::cout << "can't open config file: " << config_file << "\n";
  } else {
    po::store(po::parse_config_file(ifs, desc), vm);
    po::notify(vm);
  }

  // Check required arguments.
  if (!vm.count("bot_ip")) {
    std::cout << "Missing option bot_ip" << std::endl;
    return 0;
  }
  if (!vm.count("bot_port")) {
    std::cout << "Missing option bot_port" << std::endl;
    return 0;
  }
  if (!vm.count("db_port")) {
    std::cout << "Missing option db_port" << std::endl;
    return 0;
  }
  if (!vm.count("db_password")) {
    std::cout << "Missing option db_password" << std::endl;
    return 0;
  }

  using boost::lexical_cast;
  std::string bot_ip, bot_port, db_ip, db_port, db_pass;
  bot_ip = vm["bot_ip"].as<std::string>();
  bot_port = lexical_cast<std::string>(vm["bot_port"].as<unsigned short>());
  db_ip = vm["db_ip"].as<std::string>();
  db_port = lexical_cast<std::string>(vm["db_port"].as<unsigned short>());
  db_pass = vm["db_password"].as<std::string>();

  std::cout << "Starting server with options:\n"
            << "\tbot_ip  \t" << bot_ip << "\n"
            << "\tbot_port\t" << bot_port << "\n"
            << "\tdb_ip   \t" << db_ip << "\n"
            << "\tdb_port \t" << db_port << "\n"
            << "\tdb_pass \t" << db_pass << "\n\n";

  auto store = std::make_shared<cached_db>("db");
  boost::asio::io_service io_service;
  ws_server s(true, "packages", &io_service, store);
  http_service db_service(&io_service, store, db_ip, db_port, "admin", db_pass);
  s.start(bot_ip, bot_port);

  std::cout << "... exit\n";
}
