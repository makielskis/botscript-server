// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./dust_config.h"

#include "dust/json_util.h"

using botscript::string_map;
using botscript::command_sequence;

namespace botscript_server {

dust_config::dust_config(dust::document document)
    : doc_(std::move(document)) {
}

dust_config::dust_config(
    dust::document document,
    const std::string& json_config)
    : doc_(std::move(document)) {
  dust::insert_json(doc_, json_config);
}

dust_config::dust_config(
    dust::document document,
    const std::string& identifier,
    const std::string& username,
    const std::string& password,
    const std::string& package,
    const std::string& server,
    const std::map<std::string, string_map>& module_settings)
    : doc_(std::move(document)) {
  doc_["username"] = username;
  doc_["password"] = password;
  doc_["package"] = package;
  doc_["server"] = server;

  for (const auto& module : module_settings) {
    const std::string& module_name = module.first;
    for (const auto& settings : module.second) {
      doc_["modules"][module_name][settings.first] = settings.second;
    }
  }

  if (!doc_["modules"]["base"]["proxy"].exists() ||
      !doc_["modules"]["base"]["wait_time_factor"].exists()) {
    throw std::runtime_error("base module settings missing");
  }
}

command_sequence dust_config::init_command_sequence() const {
  command_sequence commands;

  // Base settings first.
  const string& wtf = doc_["modules"]["base"]["wait_time_factor"].val();
  auto wtf_init = make_pair("base_set_wait_time_factor", wtf);
  commands.emplace_back(move(wtf_init));

  // Iterate modules.
  for (const auto& module : doc_["modules"].children()) {
    const string& module_name = module.index();

    // Don't handle base twice.
    if (module_name == "base") {
      continue;
    }

    // Iterate module configuration values
    for (const auto& module_config : module.children()) {
      // Module activation has to be done after the configuration.
      if (module_config.val() == "active") {
        continue;
      }

      string command = module_name + "_set_" + module_config.index();
      commands.emplace_back(make_pair(command, module_config.val()));
    }

    // Last step: module activation.
    const auto it = module_config.val().find("active");
    if (it != module.second.end()) {
      commands.emplace_back(make_pair(module_name + "_set_active", it->second));
    } else {
      commands.emplace_back(make_pair(module_name + "_set_active", "0"));
    }
  }

  return commands;
}

std::string dust_config::to_json(bool with_password) const {
  // TODO(felix) implement
}

std::string dust_config::value_of(const std::string& key) const {
  // TODO(felix) implement
}

void dust_config::inactive(bool flag) {
  // TODO(felix) implement
}

bool dust_config::inactive() const {
  // TODO(felix) implement
}

std::string dust_config::identifier() const {
  // TODO(felix) implement
}

std::string dust_config::username() const {
  // TODO(felix) implement
}

std::string dust_config::password() const {
  // TODO(felix) implement
}

std::string dust_config::package() const {
  // TODO(felix) implement
}

std::string dust_config::server() const {
  // TODO(felix) implement
}

std::map<std::string, string_map> dust_config::module_settings() const {
  // TODO(felix) implement
}

void dust_config::set(const std::string& module,
                      const std::string& key,
                      const std::string& value) {
  // TODO(felix) implement
}

void dust_config::set(const std::string& key,
                      const std::string& value) {
  // TODO(felix) implement
}

}  // namespace botscript_server
