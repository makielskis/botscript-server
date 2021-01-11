// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPTIONS_PARSER_H_
#define BOTSCRIPT_SERVER_OPTIONS_PARSER_H_

#include <ostream>
#include <string>
#include <vector>

#include "boost/program_options.hpp"

#include "./configuration.h"

namespace botscript_server {

class options_parser {
public:
  options_parser(std::vector<configuration*> options);

  void configure_description();
  void read_command_line_args(int argc, char* argv[]);
  void read_configuration_file();
  bool help();
  void print_help(std::ostream& out);
  bool version();
  void print_used(std::ostream& out);
  void print_unrecognized(std::ostream& out);

private:
  std::string file_;
  boost::program_options::options_description desc_;
  std::vector<std::string> unrecog_;
  boost::program_options::variables_map vm_;
  std::vector<configuration*> options_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_OPTIONS_PARSER_H_
