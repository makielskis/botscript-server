// Copyright (c) 2014, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_DUST_SERVER_OPTIONS_H_
#define BOTSCRIPT_SERVER_DUST_SERVER_OPTIONS_H_

#include <ostream>
#include <string>

#include "dust-server/options.h"

namespace boost {
namespace program_options {
class options_description;
class variables_map;
}
}

namespace botscript_server {

class dust_server_options: public dust_server::options {
 public:
  friend std::ostream& operator<<(std::ostream& out,
                                  const dust_server_options& options);

  dust_server_options(std::string host, std::string port, std::string password);
  virtual ~dust_server_options();

  void configure_description(boost::program_options::options_description& desc);
  void parse(const boost::program_options::variables_map& vm);
};

std::ostream& operator<<(std::ostream& out, const dust_server_options& options);

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_DUST_SERVER_OPTIONS_H_
