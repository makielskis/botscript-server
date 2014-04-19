// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./options_parser.h"

#include <istream>
#include <fstream>

namespace po = boost::program_options;

namespace botscript_server {

options_parser::options_parser(std::vector<configuration*> options)
: desc_("General"),
options_(std::move(options)) {
  configure_description();
}

void options_parser::configure_description() {
  desc_.add_options()
      ("help", "produce help message")
      ("config,c",
          po::value<std::string>(&file_)->default_value("config.ini"),
          "config path");

  for (auto& option : options_) {
    desc_.add(option->desc());
  }
}

void options_parser::read_command_line_args(int argc, char* argv[]) {
  po::positional_options_description p;
  p.add("config", -1);

  auto parsed = po::command_line_parser(argc, argv)
                  .options(desc_)
                  .positional(p)
                  .allow_unregistered()
                  .run();
  po::store(parsed, vm_);
  po::notify(vm_);

  auto unrecog_cmdl = po::collect_unrecognized(parsed.options,
                                               po::include_positional);
  unrecog_.insert(unrecog_.end(), unrecog_cmdl.begin(), unrecog_cmdl.end());
}

void options_parser::read_configuration_file() {
  std::ifstream ifs(file_.c_str());
  if (ifs) {
    auto parsed_file_options = po::parse_config_file(ifs, desc_, true);

    // Add unrecognized file options to the global unrecognized options.
    auto unrecog_file = po::collect_unrecognized(parsed_file_options.options,
                                                 po::include_positional);
    unrecog_.insert(unrecog_.end(), unrecog_file.begin(), unrecog_file.end());

    po::store(parsed_file_options, vm_);
    po::notify(vm_);
  }
}

bool options_parser::help() {
  return vm_.count("help");
}

void options_parser::print_used(std::ostream& out) {
  out << "Used Options: " << "\n\n";
  for (auto const* option : options_) {
    out << *option << "\n\n";
  }
}

void options_parser::print_help(std::ostream& out) {
  out << desc_ << "\n";
}

void options_parser::print_unrecognized(std::ostream& out) {
  if (unrecog_.empty()) {
    return;
  }

  out << "Unrecognized Options:\n";
  for (auto& unrecog_option : unrecog_) {
    out << "  " << unrecog_option << "\n";
  }
  out << "\n";
}

}  // namespace botscript_server
