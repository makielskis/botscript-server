// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./message.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <string>
#include <map>

namespace botscript_server {

namespace json = rapidjson;
namespace bs = botscript;

sid_login_message::sid_login_message(const json::Document& doc) {
  sid_ = doc["arguments"]["sid"].GetString();
}

bool sid_login_message::fits(const json::Document& doc) {
  return std::string(doc["type"].GetString()) == "login"
         && doc["arguments"].HasMember("sid")
         && doc["arguments"]["sid"].IsString();
}

create_message::create_message(const json::Document& doc) {
  config_ = doc["arguments"]["config"].GetString();
}

bool create_message::fits(const json::Document& doc) {
  return std::string(doc["type"].GetString()) == "bot_management"
         && doc["arguments"].HasMember("type")
         && doc["arguments"]["type"].IsString()
         && std::string(doc["arguments"]["type"].GetString()) == "create"
         && doc["arguments"].HasMember("config")
         && doc["arguments"]["config"].IsString();
}

delete_message::delete_message(const json::Document& doc) {
  identifier_ = doc["arguments"]["identifier"].GetString();
}

bool delete_message::fits(const json::Document& doc) {
  return std::string(doc["type"].GetString()) == "bot_management"
         && doc["arguments"].HasMember("type")
         && doc["arguments"]["type"].IsString()
         && std::string(doc["arguments"]["type"].GetString()) == "delete"
         && doc["arguments"].HasMember("identifier")
         && doc["arguments"]["identifier"].IsString();
}

execute_message::execute_message(const json::Document& doc) {
  identifier_ = doc["arguments"]["identifier"].GetString();
  command_ = doc["arguments"]["execute"]["command"].GetString();
  argument_ = doc["arguments"]["execute"]["argument"].GetString();
}

bool execute_message::fits(const json::Document& doc) {
  return std::string(doc["type"].GetString()) == "bot"
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
