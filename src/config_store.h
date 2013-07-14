// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef CONFIG_STORE_H
#define CONFIG_STORE_H

#include <string>
#include <vector>

#include "bot.h"

namespace botscript_server {

/// Bot storage adapter interface that can be implemented.
/// For instance there could be:
///   - a database implementation
///   - an in memory implementation
///   - a file based implementation
class config_store {
 public:
  /// Adds a new bot configuration to the store
  ///
  /// \param configuration the configuration to store
  virtual void add(const botscript::bot& bot) = 0;

  /// Removes the configuration with the given identifier
  ///
  /// \param identifier the identifier to remove the config for
  virtual void remove(const std::string& identifier) = 0;

  /// Get the configuration for the given identifier.
  ///
  /// \return the configuration of the bot with the given identifier
  virtual std::string get(const std::string& identifier) = 0;

  /// Get all configurations in the store.
  ///
  /// \return vector containing all configurations in the store
  virtual std::vector<std::string> get_all() = 0;

  /// Updates a single attribute of a bot and sets it to the given value.
  /// Basic configuration values (username, password, ...) cannot be changed.
  ///
  /// \param bot the bot to update
  /// \param module which module to update
  /// \param key which key of the module to update
  /// \param new_value the new value for the attribute to update
  virtual void update_attribute(const botscript::bot& bot,
                                const std::string& module,
                                const std::string& key,
                                const std::string& new_value) = 0;
};

}  // namespace botscript_server

#endif  // CONFIG_STORE_H
