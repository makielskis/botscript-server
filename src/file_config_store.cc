// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include <cstdio>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

#include "file_config_store.h"

namespace bs = botscript;

namespace botscript_server {

file_config_store::file_config_store(const std::string& path)
    : config_dir_(path) {
  if (!boost::filesystem::is_directory(path)) {
    boost::filesystem::create_directories(path);
  }
}

void file_config_store::add(const bs::bot& bot) {
  try {
    std::ofstream file;
    file.open(config_dir_ + "/" + bot.identifier() + ".json", std::ios::out);
    file << bot.configuration(true);
    file.close();
  } catch (const std::exception& e) {
    // TODO: proper errorhandling
    std::cerr << e.what() << "\n";
  }
};

void file_config_store::remove(const std::string& identifier) {
  const char* filename = (config_dir_ + "/" + identifier + ".json").c_str();
  int ret_code = std::remove(filename);
  if (ret_code != 0) {
    perror(filename);
  }
};

std::string file_config_store::get(const std::string& identifier) {
  std::ifstream ifs(config_dir_ + "/" + identifier + ".json");
  std::stringstream buf;
  buf << ifs.rdbuf();
  return buf.str();
};

std::vector<std::string> file_config_store::get_all() {
  std::vector<std::string> configs;

  using boost::filesystem::directory_iterator;
  for (auto i = directory_iterator(config_dir_); i != directory_iterator(); ++i) {
    auto file = i->path();

    // Skip non-json files.
    if (file.extension() != ".json") {
      continue;
    }

    // Get file content.
    std::string identifier = file.stem().string();
    configs.emplace_back(get(identifier));
  }

  return configs;
}

void file_config_store::update_attribute(const bs::bot& bot,
                              const std::string& module,
                              const std::string& key,
                              const std::string& new_value) {
  add(bot);
};

}  // namespace botscript_server

