// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef CONFIG_STORE_H
#define CONFIG_STORE_H

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

  /// Removes all keys that start with the given identifier
  virtual void remove(const std::string& identifier) = 0;

  /// Reads all values from the database that start with the given identifier
  /// and creates a bot_config object from it.
  ///
  /// \return the configuration of the bot with the given identifier
  virtual std::string get(const std::string& identifier) = 0;

  /// Updates a single attribute (specified by module + key) of a bot (specified
  /// by the identifier) and sets it to the given value. Basic configuration
  /// values (username, password, package, ...) cannot be changed.
  virtual void update_attribute(const botscript::bot& bot,
                                const std::string& module,
                                const std::string& key,
                                const std::string& new_value) = 0;
};

}  // namespace botscript_server

#endif  // CONFIG_STORE_H
