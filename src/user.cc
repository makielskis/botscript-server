// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "user.h"

#include "boost/lexical_cast.hpp"

#include "websocketpp/common/md5.hpp"

#include "./db_bot_config.h"
#include "./error.h"

namespace botscript_server {

using websocketpp::md5::md5_hash_hex;

user::user(dust::document doc)
    : doc_(std::move(doc)) {
  if (!exists()) {
    throw boost::system::system_error(error::user_not_found);
  }
}

user::user(dust::document doc,
           const std::string& password,
           const std::string& email)
    : doc_(std::move(doc)) {
  if (exists()) {
    throw boost::system::system_error(error::username_already_taken);
  }

  doc_["password"] = md5_hash_hex(password);
  doc_["email"] = email;
  new_session();
}

bool user::exists() const {
  auto password_doc = doc_["password"];
  auto email_doc = doc_["email"];
  auto session_doc = doc_["session"];
  return password_doc.exists() || email_doc.exists() || session_doc.exists();
}

std::string user::username() const {
  return doc_.index();
}

std::string user::email() const {
  return doc_["email"].val();
}

std::vector<std::shared_ptr<botscript::bot_config>> user::bot_configs() const {
  std::vector<std::shared_ptr<botscript::bot_config>> configs;
  for (auto& bot : doc_["bots"].children()) {
    try {
      configs.push_back(std::make_shared<db_bot_config>(bot));
    } catch (std::exception const& e) {
      std::cout << "invalid configuration: " << bot.full_path();
      bot.remove();
    }
  }
  return configs;
}

void user::remove_bot(const std::string& identifier) {
  doc_["bots"][identifier].remove();
}

bool user::has_bot(const std::string& identifier) const {
  return doc_["bots"][identifier].exists();
}

std::shared_ptr<botscript::bot_config> user::bot_config(const std::string& identifier) const {
  return std::make_shared<db_bot_config>(doc_["bots"][identifier]);
}

std::string user::session_id() const {
  return doc_["session"]["id"].val();
}

std::time_t user::session_expire() const {
  return boost::lexical_cast<time_t>(doc_["session"]["expire"].val());
}

bool user::session_active() const {
  std::time_t now = std::time(nullptr);
  return session_expire() >= now;
}

void user::new_session() {
  std::time_t t = std::time(nullptr);
  std::string now_str = boost::lexical_cast<std::string>(t);
  doc_["session"]["id"] = md5_hash_hex(doc_["password"].val() + now_str);
  renew_session();
}

void user::renew_session() {
  std::time_t t = std::time(nullptr);
  doc_["session"]["expire"] = boost::lexical_cast<std::string>(t + EXPIRE);
}

void user::password(const std::string& old_pw, const std::string& new_pw) {
  if (!check_password(old_pw)) {
    throw boost::system::system_error(error::password_wrong);
  }

  doc_["password"] = md5_hash_hex(new_pw);
}

void user::email(const std::string& pw, const std::string& new_email) {
  if (!check_password(pw)) {
    throw boost::system::system_error(error::password_wrong);
  }

  doc_["email"] = new_email;
}

bool user::check_password(const std::string& password) {
  return md5_hash_hex(password) == doc_["password"].val();
}

void user::remove() {
  doc_.remove();
}

}  // namespace botscript_server
