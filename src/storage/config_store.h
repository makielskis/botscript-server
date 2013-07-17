// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef CONFIG_STORE_H
#define CONFIG_STORE_H

#include <string>
#include <vector>
#include <functional>
#include <exception>

#include "boost/optional.hpp"

#include "bot.h"

namespace botscript_server {

/// Bot storage adapter interface that can be implemented.
/// For instance there could be:
///   - a database implementation
///   - an in memory implementation
///   - a file based implementation
class config_store {
 public:
  /// Error indicator: contains an exception if the op. was not successful.
  typedef boost::optional<std::exception> error_indicator;

  /// Callback function for asynchronous operations without return value.
  typedef std::function<void (error_indicator)> empty_cb;

  /// Callback function for asynchronous operations with return values.
  template <class ReturnType>
  struct cb {
    /// Callback function used to pass returnvalues/errors asynchronously.
    typedef std::function<void (ReturnType, error_indicator)> type;
  };

  /// Adds a new bot configuration to the store
  ///
  /// \param bot  the bot to store the configuration for
  /// \param cb   the callback that will be called on operation finish
  virtual void add(const botscript::bot& bot, empty_cb cb) = 0;

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
  virtual void update_attribute(const botscript::bot& bot,
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

#endif  // CONFIG_STORE_H
