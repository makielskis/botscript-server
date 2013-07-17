// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef FILE_CONFIG_STORE_H
#define FILE_CONFIG_STORE_H

#include <thread>

#include "boost/asio/io_service.hpp"

#include "config_store.h"

namespace botscript_server {

/// File based implementation of the config_store interface.
class file_config_store : public config_store {
 public:
  /// Create/Open a file config store in the given path.
  ///
  /// \throws boost::filesystem_error
  /// \return all configurations in the path in a vector
  file_config_store(const std::string& path);

  virtual ~file_config_store();

  virtual void add(const botscript::bot& bot, empty_cb cb) override;
  virtual void remove(const std::string& identifier, empty_cb cb) override;
  virtual void get(const std::string& identifier,
                   cb<std::string>::type cb) override;
  virtual std::vector<std::string> get_all() override;
  virtual void update_attribute(const botscript::bot& bot,
                                const std::string& module,
                                const std::string& key,
                                const std::string& new_value,
                                empty_cb callback) override;

 private:
  /// Path to the configuration directory.
  const std::string config_dir_;

  /// Thread that executes I/O operations.
  std::thread execution_thread_;

  /// Service managing I/O operations.
  boost::asio::io_service io_service_;
};

}  // namespace botscript_server

#endif  // FILE_CONFIG_STORE_H
