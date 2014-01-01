// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_VERSION_H_
#define BOTSCRIPT_SERVER_VERSION_H_

#include <sstream>

#define BOTSCRIPT_SERVER_MAJOR_VERSION 0
#define BOTSCRIPT_SERVER_MINOR_VERSION 2
#define BOTSCRIPT_SERVER_PATCH_VERSION 2
#define MOBILE_UI_URL "http://192.168.178.46:8080"

namespace botscript_server {

std::string version() {
  std::ostringstream v;
  v << BOTSCRIPT_SERVER_MAJOR_VERSION << "."
    << BOTSCRIPT_SERVER_MINOR_VERSION << "."
    << BOTSCRIPT_SERVER_PATCH_VERSION;
  return v.str();
}

}  // namespace botscript_server

#endif /* BOTSCRIPT_SERVER_VERSION_H_ */
