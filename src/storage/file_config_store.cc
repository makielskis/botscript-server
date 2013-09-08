// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include <cerrno>
#include <iostream>
#include <fstream>
#include <system_error>

#include <boost/filesystem.hpp>

#include "file_config_store.h"

namespace bs = botscript;

namespace botscript_server {

file_config_store::file_config_store(const std::string& path,
                                     boost::asio::io_service* io_service)
    : config_dir_(path),
      io_service_(io_service) {
  // Create config dir if not available.
  if (!boost::filesystem::is_directory(path)) {
    boost::filesystem::create_directories(path);
  }
}

void file_config_store::io_service(boost::asio::io_service* io_service) {
  io_service_ = io_service;
}

void file_config_store::add(std::shared_ptr<bs::bot> bot, empty_cb cb) {
  io_service_->post([this, bot, cb]() {
    try {
      std::ofstream file;
      file.exceptions(std::ios_base::failbit);
      file.open(config_dir_ + "/" + bot->identifier() + ".json", std::ios::out);
      file << bot->configuration(true);
      return cb(boost::system::error_code());
    } catch(const std::ios_base::failure& e) {
      return cb(boost::system::error_code(EIO, boost::system::system_category()));
    }
  });
};

void file_config_store::remove(const std::string& identifier, empty_cb cb) {
  io_service_->post([this, identifier, cb]() {
    try {
      boost::filesystem::remove(config_dir_ + "/" + identifier + ".json");
      return cb(boost::system::error_code());
    } catch(const boost::filesystem::filesystem_error& e) {
      return cb(boost::system::error_code(EIO, boost::system::system_category()));
    }
  });
};

void file_config_store::get(const std::string& identifier,
                            cb<std::string>::type cb) {
  io_service_->post([this, identifier, cb]() {
    try {
      std::ifstream file(config_dir_ + "/" + identifier + ".json");
      file.exceptions(std::ios_base::failbit);
      std::stringstream buf;
      buf << file.rdbuf();
      return cb(buf.str(), boost::system::error_code());
    } catch (const std::ios_base::failure& e) {
      return cb("", boost::system::error_code(EIO, boost::system::system_category()));
    }
  });
};

void file_config_store::get(const std::vector<std::string>& identifiers,
                          cb<std::vector<std::string>>::type cb) {
  io_service_->post([=]() {
    try {
      std::vector<std::string> configurations;
      for (const auto& identifier : identifiers) {
        std::ifstream file(config_dir_ + "/" + identifier + ".json");
        file.exceptions(std::ios_base::failbit);
        std::stringstream buf;
        buf << file.rdbuf();
        configurations.emplace_back(buf.str());
      }
      return cb(configurations, boost::system::error_code());
    } catch (const std::ios_base::failure& e) {
      return cb(std::vector<std::string>(),
                boost::system::error_code(EIO, boost::system::system_category()));
    }
  });
}

void file_config_store::update_attribute(std::shared_ptr<bs::bot> bot,
                                         const std::string& module,
                                         const std::string& key,
                                         const std::string& new_value,
                                         empty_cb cb) {
  add(bot, [cb](boost::system::error_code e) { return cb(e); });
};

std::vector<std::string> file_config_store::get_all() {
  std::vector<std::string> configs;

  // Iterate all files from the config directory.
  using boost::filesystem::directory_iterator;
  auto i = directory_iterator(config_dir_);
  for (; i != directory_iterator(); ++i) {
    auto file = i->path();

    // Skip non-json files.
    if (file.extension() != ".json") {
      continue;
    }

    // Get file content.
    std::ifstream ifs(file.string());
    ifs.exceptions(std::ios_base::failbit);
    std::stringstream buf;
    buf << ifs.rdbuf();
    configs.emplace_back(buf.str());
  }

  return configs;
}

void file_config_store::set_inactive(
    const std::string& identifier,
    bool flag,
    empty_cb cb) {
  io_service_->post([this, cb, identifier, flag]() {
    try {
      std::stringstream buf;

      {
        std::ifstream in_file(config_dir_ + "/" + identifier + ".json");
        in_file.exceptions(std::ios_base::failbit);
        buf << in_file.rdbuf();
      }

      bs::config c(buf.str());
      c.inactive(flag);

      {
        std::ofstream file;
        file.exceptions(std::ios_base::failbit);
        file.open(config_dir_ + "/" + identifier + ".json", std::ios::out);
        file << c.to_json(true);
      }

      return cb(boost::system::error_code());
    } catch (const std::runtime_error& e) {
      return cb(boost::system::error_code(EIO, boost::system::system_category()));
    } catch (const std::ios_base::failure& e) {
      return cb(boost::system::error_code(EIO, boost::system::system_category()));
    }
  });
}

void file_config_store::get_inactive(
    const std::string& identifier,
    cb<bool>::type cb) {
  io_service_->post([this, identifier, cb]() {
    try {
      std::ifstream in_file(config_dir_ + "/" + identifier + ".json");
      in_file.exceptions(std::ios_base::failbit);

      std::stringstream buf;
      buf << in_file.rdbuf();

      bs::config c(buf.str());

      return cb(c.inactive(), boost::system::error_code());
    } catch (const std::runtime_error& e) {
      return cb("", boost::system::error_code(EIO, boost::system::system_category()));
    } catch (const std::ios_base::failure& e) {
      return cb("", boost::system::error_code(EIO, boost::system::system_category()));
    }
  });
}

}  // namespace botscript_server

