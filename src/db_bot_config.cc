// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./db_bot_config.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "dust/json_util.h"
#include "dust/json_visitor.h"

using botscript::string_map;
using botscript::command_sequence;

namespace botscript_server {

db_bot_config::db_bot_config(dust::document document)
    : doc_(std::move(document)) {
}

db_bot_config::db_bot_config(
    dust::document document,
    const std::string& json_config)
    : doc_(std::move(document)) {
  dust::insert_json(doc_, json_config);
  if (!valid()) {
    doc_.remove();
    throw std::runtime_error("invalid configuration");
  }
}

db_bot_config::db_bot_config(
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

  if (!valid()) {
    doc_.remove();
    throw std::runtime_error("invalid configuration");
  }
}

command_sequence db_bot_config::init_command_sequence() const {
  command_sequence commands;

  // Base settings first.
  const std::string& wtf = doc_["modules"]["base"]["wait_time_factor"].val();
  auto wtf_init = make_pair("base_set_wait_time_factor", wtf);
  commands.emplace_back(move(wtf_init));

  // Iterate modules.
  for (const auto& module : doc_["modules"].children()) {
    const std::string& module_name = module.index();

    // Don't handle base twice.
    if (module_name == "base") {
      continue;
    }

    // Iterate module configuration values
    std::string active = "0";
    for (const auto& module_config : module.children()) {
      // Module activation has to be done after the configuration.
      if (module_config.val() == "active") {
        active = module_config.val();
        continue;
      }

      std::string command = module_name + "_set_" + module_config.index();
      commands.emplace_back(make_pair(command, module_config.val()));
    }

    // Add active state command after all other settings have been applied.
    commands.emplace_back(make_pair(module_name + "_set_active", active));
  }

  return commands;
}

std::string db_bot_config::to_json(bool with_password) const {
  typedef rapidjson::Writer<rapidjson::StringBuffer> JsonStringWriter;

  rapidjson::StringBuffer buffer;
  JsonStringWriter writer(buffer);
  dust::json_visitor<JsonStringWriter> json(writer);
  if (!with_password) {
    json.ignore("password");
  }
  doc_.accept(json);
  return buffer.GetString();
}

std::string db_bot_config::value_of(const std::string& key) const {
  auto pos = key.find("_");
  if (pos != std::string::npos) {
    std::string module = key.substr(0, pos);
    std::string setting = key.substr(pos + 1);

    dust::document val = doc_["modules"][module][setting];
    if (val.exists()) {
      return val.val();
    }
  }
  return "";
}

void db_bot_config::inactive(bool flag) {
  doc_["inactive"] = flag ? "1" : "0";
}

bool db_bot_config::inactive() const {
  return doc_["inactive"].exists() ? (doc_["inactive"].val() == "1") : false;
}

std::string db_bot_config::identifier() const {
  return doc_["identifier"].exists() ? doc_["identifier"].val() : "";
}

std::string db_bot_config::username() const {
  return doc_["username"].exists() ? doc_["username"].val() : "";
}

std::string db_bot_config::password() const {
  return doc_["password"].exists() ? doc_["password"].val() : "";
}

std::string db_bot_config::package() const {
  return doc_["package"].exists() ? doc_["package"].val() : "";
}

std::string db_bot_config::server() const {
  return doc_["server"].exists() ? doc_["server"].val() : "";
}

std::map<std::string, string_map> db_bot_config::module_settings() const {
  std::map<std::string, string_map> module_settings;
  for (const auto& module : doc_["modules"].children()) {
    std::string module_name = module.index();
    for (const auto& module_config : module.children()) {
      module_settings[module_name][module_config.index()] = module_config.val();
    }
  }
  return module_settings;
}

void db_bot_config::set(const std::string& module,
                        const std::string& key,
                        const std::string& value) {
  doc_["modules"][module][key] = value;
}

void db_bot_config::set(const std::string& key,
                        const std::string& value) {
  auto pos = key.find("_");
  if (pos != std::string::npos) {
    std::string module = key.substr(0, pos);
    std::string setting = key.substr(pos + 1);
    doc_["modules"][module][setting] = value;
  }
}

}  // namespace botscript_server
