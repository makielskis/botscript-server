// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./bots_msg.h"

#include <sstream>

namespace botscript_server {

bots_msg::bots_msg(const std::map<std::string, std::string>& bot_configs)
    : bot_configs_(bot_configs) {
}

std::string bots_msg::to_json() const {
  std::stringstream s;
  for (const auto& config : bot_configs_) {
    s << ",\"" << config.first << "\":" << config.second;
  }
  return "{\"type\":\"bots\",\"arguments\":{" + s.str().substr(1) + "}}";
}

}  // namespace botscript_server
