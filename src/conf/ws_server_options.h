// Copyright (c) 2014, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_WS_SERVER_OPTIONS_H_
#define BOTSCRIPT_SERVER_WS_SERVER_OPTIONS_H_

#include <ostream>
#include <string>

#include "./configuration.h"

namespace botscript_server {

class ws_server_options : public configuration {
public:
  friend std::ostream& operator<<(std::ostream& out,
                                  const ws_server_options& options);

  ws_server_options(std::string host_default, std::string port_default);

  virtual ~ws_server_options() {}

  virtual boost::program_options::options_description desc() override;
  virtual void print(std::ostream& out) const override;

  std::string host;
  std::string port;
};

std::ostream& operator<<(std::ostream& out, const ws_server_options& options);

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_WS_SERVER_OPTIONS_H_
