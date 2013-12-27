// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include <iostream>

#include "dust/storage/cached_db.h"

#include "bot.h"

#include "./make_unique.h"
#include "./ws_server.h"

using namespace botscript_server;
using namespace dust;
using namespace botscript;

int main() {
  auto store = make_unique<cached_db>("./db");
  ws_server s(std::move(store), bot::load_packages("./packages"));
  s.start("127.0.0.1", "9003");
  std::cout << "... exit\n";
}
