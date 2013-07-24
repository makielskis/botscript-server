// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DB_CONFIG_STORE_H_
#define DB_CONFIG_STORE_H_

#include "config_store.h"

#include "boost/asio/io_service.hpp"

#include "./kc_wrapper.h"

namespace botscript_server {

/// File based implementation of the config_store interface.
class db_config_store : public config_store {
 public:
  /// Create/Open a file config stored in the given path.
  ///
  /// \throws boost::filesystem_error
  /// \return all configurations in the path in a vector
  db_config_store(const std::string& path, boost::asio::io_service* io_service);

  /// \param io_service  the io_service to set
  void io_service(boost::asio::io_service* io_service);

  virtual void add(std::shared_ptr<botscript::bot> bot, empty_cb cb) override;
  virtual void remove(const std::string& identifier, empty_cb cb) override;
  virtual void get(const std::string& identifier,
                   cb<std::string>::type cb) override;
  virtual std::vector<std::string> get_all() override;
  virtual void update_attribute(std::shared_ptr<botscript::bot> bot,
                                const std::string& module,
                                const std::string& key,
                                const std::string& new_value,
                                empty_cb cb) override;

 private:
  /// Syncronous helper version of get()
  std::string get_sync(const std::string& identifier);

  /// Service managing I/O operations.
  boost::asio::io_service* io_service_;

  /// Kyoto Cabinet wrapper handle.
  db db_;

  /// DB default entry for bots
  entry bots_;
};

}  // namespace botscript_server

#endif  // DB_CONFIG_STORE_H_
