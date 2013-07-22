// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef  CONFIG_STORE_H_
#define  CONFIG_STORE_H_

#include <string>
#include <vector>
#include <functional>
#include <exception>
#include <memory>

#include "boost/optional.hpp"

#include "bot.h"

#include "./callback.h"

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
  /// \param bot  the bot to store the configuration for
  /// \param cb   the callback that will be called on operation finish
  virtual void add(std::shared_ptr<botscript::bot> bot, empty_cb cb) = 0;

  /// Removes the configuration with the given identifier
  ///
  /// \param identifier  the identifier to remove the config for
  /// \param cb          the callback that will be called on operation finish
  virtual void remove(const std::string& identifier, empty_cb cb) = 0;

  /// Get the configuration for the given identifier.
  ///
  /// \param identifier  the identifier of the bot configuration to get
  /// \param cb          the callback that will be called on operation finish
  ///                    with the configuration of the bot
  virtual void get(const std::string& identifier, cb<std::string>::type cb) = 0;

  /// Updates a single attribute of a bot and sets it to the given value.
  /// Basic configuration values (username, password, ...) cannot be changed.
  ///
  /// \param bot        the bot to update
  /// \param module     which module to update
  /// \param key which  key of the module to update
  /// \param new_value  the new value for the attribute to update
  /// \param cb         the callback that will be called on operation finish
  virtual void update_attribute(std::shared_ptr<botscript::bot> bot,
                                const std::string& module,
                                const std::string& key,
                                const std::string& new_value,
                                empty_cb callback) = 0;

  /// Get all configurations in the store.
  ///
  /// \return vector containing all configurations in the store
  virtual std::vector<std::string> get_all() = 0;
};

}  // namespace botscript_server

#endif  //  CONFIG_STORE_H_
