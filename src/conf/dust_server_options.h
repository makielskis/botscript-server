// Copyright (c) 2014, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_DUST_SERVER_OPTIONS_H_
#define BOTSCRIPT_SERVER_DUST_SERVER_OPTIONS_H_

#include <ostream>
#include <string>

#include "dust-server/options.h"

#include "./configuration.h"

namespace botscript_server {

class dust_server_options : public dust_server::options, public configuration {
public:
  friend std::ostream& operator<<(std::ostream& out,
                                  const dust_server_options& options);

  dust_server_options(std::string host, std::string port, std::string password);

  virtual ~dust_server_options() {
  }

  virtual boost::program_options::options_description desc() override;
};

std::ostream& operator<<(std::ostream& out, const dust_server_options& options);

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_DUST_SERVER_OPTIONS_H_
