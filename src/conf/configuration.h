// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_CONFIGURATION_H_
#define BOTSCRIPT_SERVER_CONFIGURATION_H_

namespace boost {
namespace program_options {
class options_description;
class variables_map;
}
}

namespace botscript_server {

class configuration {
public:
  virtual ~configuration() { }
  virtual boost::program_options::options_description desc() = 0;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_CONFIGURATION_H_
