// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef FILE_CONFIG_STORE_H
#define FILE_CONFIG_STORE_H

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

  virtual void add(const botscript::bot& bot) override;

  virtual void remove(const std::string& identifier) override;

  virtual std::string get(const std::string& identifier) override;

  virtual std::vector<std::string> get_all() override;

  virtual void update_attribute(const botscript::bot& bot,
                                const std::string& module,
                                const std::string& key,
                                const std::string& new_value) override;

 private:
  const std::string config_dir_;
};

}  // namespace botscript_server

#endif  // FILE_CONFIG_STORE_H
