// Copyright (c) 2014, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_BS_SERVER_OPTIONS_H_
#define BOTSCRIPT_SERVER_BS_SERVER_OPTIONS_H_

#include <ostream>
#include <string>

#include "./configuration.h"

namespace botscript_server {

class bs_server_options : public configuration {
public:
  friend std::ostream& operator<<(std::ostream& out,
                                  const bs_server_options& options);

  bs_server_options(bool forceproxy_default, bool autologin_default,
                    std::string default_packages_path, bool botlog_default);

  virtual ~bs_server_options() {
  }

  virtual boost::program_options::options_description desc() override;
  virtual void print(std::ostream& out) const override;

  bool forceproxy;
  bool autologin;
  std::string packages_path;
  bool botlog;
  std::string allowed_users;
};

std::ostream& operator<<(std::ostream& out, const bs_server_options& options);

}  // botscript_server

#endif  // BOTSCRIPT_SERVER_BS_SERVER_OPTIONS_H_
