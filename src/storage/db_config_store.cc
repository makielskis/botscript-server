// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "db_config_store.h"

#include <string>

#include "bot.h"
#include "config.h"

#define DB_KEY_PREFIX "bot"

namespace bs = botscript;

namespace botscript_server {

db_config_store::db_config_store(const std::string& path,
                                 boost::asio::io_service* io_service)
  : io_service_(io_service),
    db_(path),
    bots_(db_.get_entry(DB_KEY_PREFIX)) {
}

void db_config_store::io_service(boost::asio::io_service* io_service) {
  io_service_ = io_service;
}

void db_config_store::add(std::shared_ptr<botscript::bot> bot, empty_cb cb) {
  io_service_->post([=]() {
    const std::string& identifier = bot->identifier();
    bs::config config(bot->configuration(true));

    bots_[identifier]["username"] = config.username();
    bots_[identifier]["password"] = config.password();
    bots_[identifier]["package"] = config.package();
    bots_[identifier]["server"] = config.server();

    const auto& module_config = config.module_settings();
    for (const auto& module : module_config) {
      const std::string& module_name = module.first;
      for (const auto& settings : module.second) {
        const std::string& key = settings.first;
        const std::string& value = settings.second;
        bots_[identifier]["module"][module_name][key] = value;
      }
    }

    return cb(error_indicator());
  });
};

void db_config_store::remove(const std::string& identifier, empty_cb cb) {
  io_service_->post([=]() {
    db_.remove_prefix({DB_KEY_PREFIX, identifier});
    return cb(error_indicator());
  });
};

void db_config_store::get(const std::string& identifier,
                          cb<std::string>::type cb) {
  io_service_->post([=]() {
    return cb(get_sync(identifier), error_indicator());
  });
};

std::vector<std::string> db_config_store::get_all() {
  // Get all bot db entrys.
  std::vector<std::string> keys = db_.match_prefix({DB_KEY_PREFIX});

  // Store all configs in vector.
  std::vector<std::string> configs;
  std::unordered_set<std::string> done_identifiers;
  for (const auto& key : keys) {
    std::vector<std::string> path_elements;
    boost::split(path_elements, key, boost::is_any_of("#"));
    const std::string& identifier = path_elements.at(1);

    // Add each config once.
    if (done_identifiers.find(identifier) == done_identifiers.end()) {
      configs.emplace_back(get_sync(identifier));
      done_identifiers.insert(identifier);
    }
  }

  return configs;
};

void db_config_store::update_attribute(std::shared_ptr<botscript::bot> bot,
                                       const std::string& module,
                                       const std::string& key,
                                       const std::string& new_value,
                                       empty_cb cb) {
  io_service_->post([=]() {
    bots_[bot->identifier()]["module"][module][key] = new_value;
    return cb(error_indicator());
  });
};

std::string db_config_store::get_sync(const std::string& identifier) {
  // Get all module related db entrys.
  std::vector<std::string> keys = db_.match_prefix({DB_KEY_PREFIX,
                                                    identifier,
                                                    "module"});

  // Create module settings map.
  std::map<std::string, bs::config::string_map> module_settings;
  for (const auto& key : keys) {
    std::vector<std::string> path_elements;
    boost::split(path_elements, key, boost::is_any_of("#"));

    const std::string& module_name = path_elements.at(3);
    const std::string& option = path_elements.at(4);
    const std::string& value = db_.get_entry(key).val();

    module_settings[module_name][option] = value;
  }

  // Get basic information from db.
  const std::string& username = bots_[identifier]["username"].val();
  const std::string& password = bots_[identifier]["password"].val();
  const std::string& package = bots_[identifier]["package"].val();
  const std::string& server = bots_[identifier]["server"].val();

  // Create config object.
  bs::config config(username, password, package, server, module_settings);

  std::cout << config.to_json(true) << "\n";
  return config.to_json(true);
}

}  // namespace botscript_server
