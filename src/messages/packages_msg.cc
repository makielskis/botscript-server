// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./packages_msg.h"

#include <sstream>

namespace botscript_server {

packages_msg::packages_msg(const std::vector<std::string>& packages)
    : packages_(packages) {
}

const std::vector<std::string>& packages_msg::packages() const {
  return packages_;
}

std::string packages_msg::to_json() const {
  std::stringstream s;
  for (const std::string& package : packages_) {
    s << "," << package;
  }
  return "{\"type\":\"packages\",\"arguments\":[" + s.str().substr(1) + "]}";
}

}  // namespace botscript_server
