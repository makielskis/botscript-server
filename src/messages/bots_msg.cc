// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./bots_msg.h"

#include <sstream>

#include <boost/algorithm/string/replace.hpp>

namespace botscript_server {

bots_msg::bots_msg(const std::map<std::string, std::string>& bot_configs)
    : bot_configs_(bot_configs) {}

const std::map<std::string, std::string>& bots_msg::bot_configs() const {
  return bot_configs_;
}

std::string bots_msg::to_json() const {
  if (bot_configs_.empty()) {
    return "{\"type\":\"bots\",\"arguments\":{}}";
  }

  std::stringstream s;
  for (const auto& config : bot_configs_) {
    std::string bot_name = config.first;
    boost::replace_all(bot_name, "\"", "\\\"");
    s << ",\"" << bot_name << "\":" << config.second;
  }
  return "{\"type\":\"bots\",\"arguments\":{" + s.str().substr(1) + "}}";
}

}  // namespace botscript_server
