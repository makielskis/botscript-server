// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include <iostream>

#include "bot.h"

#include "./ws_server.h"

using namespace botscript_server;
namespace bs = botscript;

int main() {
  ws_server s(bs::bot::load_packages("./packages"));
  s.start("0.0.0.0", "9003");
  std::cout << "... exit\n";
}
