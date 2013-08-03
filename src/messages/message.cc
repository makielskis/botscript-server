// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./message.h"

#include <string>
#include <map>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace botscript_server {

namespace json = rapidjson;
namespace bs = botscript;

auth_login_message::auth_login_message(const rapidjson::Document& doc) {
  username_ = doc["arguments"]["username"].GetString();
  password_ = doc["arguments"]["password"].GetString();
}

bool auth_login_message::fits(const rapidjson::Document& doc) {
  return std::string(doc["type"].GetString()) == "login"
         && doc["arguments"].HasMember("username")
         && doc["arguments"]["username"].IsString()
         && doc["arguments"].HasMember("password")
         && doc["arguments"]["password"].IsString();
}

sid_login_message::sid_login_message(const json::Document& doc) {
  sid_ = doc["arguments"]["sid"].GetString();
}

bool sid_login_message::fits(const json::Document& doc) {
  return std::string(doc["type"].GetString()) == "login"
         && doc["arguments"].HasMember("sid")
         && doc["arguments"]["sid"].IsString();
}

update_account_message::update_account_message(const rapidjson::Document& doc) {
  sid_ = doc["arguments"]["sid"].GetString();
  password_ = doc["arguments"]["password"].GetString();
}

bool update_account_message::fits(const rapidjson::Document& doc) {
  return std::string(doc["type"].GetString()) == "account"
         && doc["arguments"].HasMember("sid")
         && doc["arguments"]["sid"].IsString()
         && doc["arguments"].HasMember("password")
         && doc["arguments"]["password"].IsString()
         && doc["arguments"].HasMember("update")
         && doc["arguments"]["update"].IsObject()
         && doc["arguments"]["update"].HasMember("key")
         && doc["arguments"]["update"]["key"].IsString();
}

delete_account_message::delete_account_message(const rapidjson::Document& doc)
  : update_account_message(doc) {
}

bool delete_account_message::fits(const rapidjson::Document& doc) {
  return std::string(doc["arguments"]["update"]["key"].GetString()) == "delete";
}

set_email_message::set_email_message(const rapidjson::Document& doc)
  : update_account_message(doc) {
  new_email_ = doc["arguments"]["update"]["value"].GetString();
}

bool set_email_message::fits(const rapidjson::Document& doc) {
  return std::string(doc["arguments"]["update"]["key"].GetString()) == "email"
         && doc["arguments"]["update"].HasMember("value")
         && doc["arguments"]["update"]["value"].IsString();
}

set_password_message::set_password_message(const rapidjson::Document& doc)
  : update_account_message(doc) {
  new_password_ = doc["arguments"]["update"]["value"].GetString();
}

bool set_password_message::fits(const rapidjson::Document& doc) {
  return std::string(doc["arguments"]["update"]["key"].GetString()) == "password"
         && doc["arguments"]["update"].HasMember("value")
         && doc["arguments"]["update"]["value"].IsString();
}

create_message::create_message(const json::Document& doc) {
  sid_ = doc["arguments"]["sid"].GetString();
  config_ = doc["arguments"]["config"].GetString();
}

bool create_message::fits(const json::Document& doc) {
  return std::string(doc["type"].GetString()) == "bot_management"
         && doc["arguments"].HasMember("sid")
         && doc["arguments"]["sid"].IsString()
         && doc["arguments"].HasMember("type")
         && doc["arguments"]["type"].IsString()
         && std::string(doc["arguments"]["type"].GetString()) == "create"
         && doc["arguments"].HasMember("config")
         && doc["arguments"]["config"].IsString();
}

delete_message::delete_message(const json::Document& doc) {
  sid_ = doc["arguments"]["sid"].GetString();
  identifier_ = doc["arguments"]["identifier"].GetString();
}

bool delete_message::fits(const json::Document& doc) {
  return std::string(doc["type"].GetString()) == "bot_management"
         && doc["arguments"].HasMember("sid")
         && doc["arguments"]["sid"].IsString()
         && doc["arguments"].HasMember("type")
         && doc["arguments"]["type"].IsString()
         && std::string(doc["arguments"]["type"].GetString()) == "delete"
         && doc["arguments"].HasMember("identifier")
         && doc["arguments"]["identifier"].IsString();
}

execute_message::execute_message(const json::Document& doc) {
  sid_ = doc["arguments"]["sid"].GetString();
  identifier_ = doc["arguments"]["identifier"].GetString();
  command_ = doc["arguments"]["execute"]["command"].GetString();
  argument_ = doc["arguments"]["execute"]["argument"].GetString();
}

bool execute_message::fits(const json::Document& doc) {
  return std::string(doc["type"].GetString()) == "bot"
         && doc["arguments"].HasMember("sid")
         && doc["arguments"]["sid"].IsString()
         && doc["arguments"].HasMember("identifier")
         && doc["arguments"]["identifier"].IsString()
         && doc["arguments"].HasMember("execute")
         && doc["arguments"]["execute"].IsObject()
         && doc["arguments"]["execute"].HasMember("command")
         && doc["arguments"]["execute"]["command"].IsString()
         && doc["arguments"]["execute"].HasMember("argument")
         && doc["arguments"]["execute"]["argument"].IsString();
} 

}  // namespace botscript_server
