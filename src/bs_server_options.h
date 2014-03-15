// Copyright (c) 2014, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_BS_SERVER_OPTIONS_H_
#define BOTSCRIPT_SERVER_BS_SERVER_OPTIONS_H_

#include <ostream>
#include <string>

namespace boost {
namespace program_options {
class options_description;
class variables_map;
}
}

namespace botscript_server {

class bs_server_options {
public:
  friend std::ostream& operator<<(std::ostream& out,
                                  const bs_server_options& options);

  bs_server_options(bool forceproxy_default,
                    bool autologin_default,
                    std::string default_packages_path,
                    bool botlog);

  void configure_description(boost::program_options::options_description& desc);
  void parse(const boost::program_options::variables_map& vm);

  bool forceproxy() const;
  bool autologin() const;
  std::string packages_path() const;
  bool botlog() const;
  std::string allowed_users() const;

private:
  bool forceproxy_;
  bool autologin_;
  std::string packages_path_;
  bool botlog_;
  std::string allowed_users_;
};

std::ostream& operator<<(std::ostream& out, const bs_server_options& options);

}  // botscript_server

#endif  // BOTSCRIPT_SERVER_BS_SERVER_OPTIONS_H_
