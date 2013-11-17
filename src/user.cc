// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "user.h"

#include "websocketpp/common/md5.hpp"

namespace botscript_server {

user::user(dust::document doc)
    : doc_(std::move(doc)) {
  //TODO: Check validity and throw errors.
}

user::user(dust::document doc,
           const std::string& username,
           const std::string& password)
    : doc_(std::move(doc)) {
  //TODO: Check if user exits and throw error if so.
  doc_["username"] = username;
  doc_["password"] = websocketpp::md5::md5_hash_hex(password);
}

std::string user::username() {
  return doc_["username"];
}

void user::password(const std::string& old_pw, const std::string& new_pw) {
  if (!check_password(old_pw)) {
    throw std::runtime_error("wrong password");
  }

  doc_["password"] = websocketpp::md5::md5_hash_hex(new_pw);
}

bool user::check_password(const std::string& password) {
  return websocketpp::md5::md5_hash_hex(password) == doc_["password"].val();
}



}  // namespace botscript_server

