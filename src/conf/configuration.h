// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_CONFIGURATION_H_
#define BOTSCRIPT_SERVER_CONFIGURATION_H_

#include <ostream>

namespace boost {
namespace program_options {
class options_description;
class variables_map;
}  // namespace program_options
}  // namespace boost

namespace botscript_server {

class configuration {
public:
  virtual ~configuration() {}
  virtual boost::program_options::options_description desc() = 0;
  virtual void print(std::ostream& out) const = 0;

  friend std::ostream& operator<<(std::ostream& out,
                                  configuration const& conf) {
    conf.print(out);
    return out;
  }
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_CONFIGURATION_H_
