// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef FILE_CONFIG_STORE_H_
#define FILE_CONFIG_STORE_H_

#include "boost/asio/io_service.hpp"

#include "config_store.h"

namespace botscript_server {

/// File based implementation of the config_store interface.
class file_config_store : public config_store {
 public:
  /// Create/Open a file config stored in the given path.
  ///
  /// \throws boost::filesystem_error
  /// \return all configurations in the path in a vector
  file_config_store(const std::string& path,
                    boost::asio::io_service* io_service);

  /// \param io_service  the io_service to set
  void io_service(boost::asio::io_service* io_service);

  virtual void add(std::shared_ptr<botscript::bot> bot, empty_cb cb) override;
  virtual void remove(const std::string& identifier, empty_cb cb) override;
  virtual void get(const std::string& identifier,
                   cb<std::string>::type cb) override;
  virtual void get(const std::vector<std::string>& identifiers,
                   cb<std::map<std::string, std::string>>::type cb) = 0;
  virtual std::vector<std::string> get_all() override;
  virtual void update_attribute(std::shared_ptr<botscript::bot> bot,
                                const std::string& module,
                                const std::string& key,
                                const std::string& new_value,
                                empty_cb callback) override;
  virtual void set_inactive(const std::string& identifier,
                            bool flag,
                            empty_cb cb);
  virtual void get_inactive(const std::string& identifier, cb<bool>::type cb);

 private:
  /// Path to the configuration directory.
  const std::string config_dir_;

  /// Service managing I/O operations.
  boost::asio::io_service* io_service_;
};

}  // namespace botscript_server

#endif  // FILE_CONFIG_STORE_H_
