// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include <iostream>
#include <memory>

#include "boost/asio/io_service.hpp"

#include "dust/storage/cached_db.h"

#include "bot.h"

#include "./ws_server.h"

using namespace botscript_server;
using namespace dust;
using namespace botscript;

int main() {
  auto store = std::make_shared<cached_db>("db");
  boost::asio::io_service io_service;
  ws_server s(true, &io_service, store, bot::load_packages("./packages"));
  s.start("127.0.0.1", "9003");
  std::cout << "... exit\n";
}
