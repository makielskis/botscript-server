// Copyright (c) 2014, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_WS_SERVER_OPTIONS_H_
#define BOTSCRIPT_SERVER_WS_SERVER_OPTIONS_H_

#include <ostream>
#include <string>

namespace boost {
namespace program_options {
class options_description;
class variables_map;
}
}

namespace botscript_server {

class ws_server_options {
public:
  friend std::ostream& operator<<(std::ostream& out,
                                  const ws_server_options& options);

  ws_server_options(std::string host_default, std::string port_default);

  void configure_description(boost::program_options::options_description& desc);
  void parse(const boost::program_options::variables_map& vm);

  std::string host() const;
  std::string port() const;

private:
  std::string host_;
  std::string port_;
};

std::ostream& operator<<(std::ostream& out, const ws_server_options& options);

}  // namespace botscript_server

#endif   // BOTSCRIPT_SERVER_WS_SERVER_OPTIONS_H_
