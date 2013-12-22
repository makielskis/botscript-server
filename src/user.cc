// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "user.h"

#include "websocketpp/common/md5.hpp"

#include "./db_bot_config.h"
#include "./error.h"

namespace botscript_server {

user::user(dust::document doc)
    : doc_(std::move(doc)) {
  if (doc_["password"].val().length() != 32) {
    throw boost::system::system_error(error::user_not_found);
  }
}

user::user(dust::document doc,
           const std::string& password)
    : doc_(std::move(doc)) {
  dust::document password_doc = doc["password"];

  if (password_doc.exists()) {
    throw boost::system::system_error(error::username_already_taken);
  }

  password_doc = websocketpp::md5::md5_hash_hex(password);
}

std::string user::username() {
  return doc_.index();
}

void user::password(const std::string& old_pw, const std::string& new_pw) {
  if (!check_password(old_pw)) {
    throw boost::system::system_error(error::password_wrong);
  }

  doc_["password"] = websocketpp::md5::md5_hash_hex(new_pw);
}

bool user::check_password(const std::string& password) {
  return websocketpp::md5::md5_hash_hex(password) == doc_["password"].val();
}

std::vector<std::shared_ptr<botscript::bot_config>> user::bots_config() {
  std::vector<std::shared_ptr<botscript::bot_config>> configs;
  for (const auto& bot : doc_["bots"].children()) {
    configs.push_back(std::make_shared<db_bot_config>(bot));
  }
  return configs;
}

}  // namespace botscript_server

