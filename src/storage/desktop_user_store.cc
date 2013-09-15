// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "desktop_user_store.h"

#include <iostream>
#include <fstream>

#include "boost/system/error_code.hpp"
#include "boost/algorithm/string/split.hpp"

#include "../error.h"

namespace botscript_server {

desktop_user_store::desktop_user_store(const std::string& path)
  : path_(path) {
}

void desktop_user_store::add_user(const std::string& username,
                                  const std::string& password,
                                  const std::string& email,
                                  cb<std::string>::type cb) {
  return cb("-", boost::system::error_code());
}

void desktop_user_store::remove_user(const std::string& session_id,
                                     const std::string& password,
                                     empty_cb cb) {
  return cb(boost::system::error_code());
}

void desktop_user_store::login(const std::string& username,
                               const std::string& password,
                               cb<std::string>::type cb) {
  return cb("-", boost::system::error_code());
}

void desktop_user_store::logout(const std::string& session_id, empty_cb cb) {
  return cb(boost::system::error_code());
}

void desktop_user_store::set_password(const std::string& session_id,
                                 const std::string& old_password,
                                 const std::string& new_password,
                                 empty_cb cb) {
  return cb(boost::system::error_code());
}

void desktop_user_store::get_email(const std::string& session_id,
                                   cb<std::string>::type cb) {
  return cb("-", boost::system::error_code());
}

void desktop_user_store::set_email(const std::string& session_id,
                                   const std::string& password,
                                   const std::string& email,
                                   empty_cb cb) {
  return cb(boost::system::error_code());
}

void desktop_user_store::get_bots(const std::string& session_id,
                                  cb<std::vector<std::string>>::type cb)  {
  try {
    read_bots();
  } catch(const std::ios_base::failure& e) {
    return cb(std::vector<std::string>(),
              boost::system::error_code(EIO, boost::system::system_category()));
  }
  std::vector<std::string> bots;
  for (const auto& b : bots_) {
    bots.push_back(b);
  }
  return cb(bots, boost::system::error_code());
}

void desktop_user_store::add_bot(const std::string& session_id,
                                 const std::string& identifier,
                                 empty_cb cb) {
  try {
    read_bots();

    // Check if bot already present.
    const auto bot_it = bots_.find(identifier);
    if (bot_it != bots_.end()) {
      return cb(error::bot_already_exists);
    }

    bots_.insert(identifier);

    write_bots();
  } catch(const std::ios_base::failure& e) {
    return cb(boost::system::error_code(EIO, boost::system::system_category()));
  }
  return cb(boost::system::error_code());
}

void desktop_user_store::remove_bot(const std::string& session_id,
                                    const std::string& identifier,
                                    empty_cb cb) {
  try {
    read_bots();

    const auto bot_it = bots_.find(identifier);
    if (bot_it == bots_.end()) {
      return cb(error::bot_not_found);
    }

    bots_.erase(identifier);

    write_bots();
  } catch(const std::ios_base::failure& e) {
    return cb(boost::system::error_code(EIO, boost::system::system_category()));
  }
  return cb(boost::system::error_code());
}

void desktop_user_store::read_bots() {
  std::ifstream file(path_);
  file.exceptions(std::ios_base::failbit);
  std::stringstream buf;
  buf << file.rdbuf();
  std::string bots = buf.str();

  std::vector<std::string> split_vec;
  boost::split(split_vec, bots, boost::is_any_of(","));

  bots_.clear();
  for (const auto& el : split_vec) {
    if (!el.empty()) {
      bots_.insert(el);
    }
  }
}

void desktop_user_store::write_bots() {
  std::string bots;

  if (bots_.empty()) {
    bots = "";
  } else {
    std::string bots;
    for (const std::string& el : bots_) {
      bots += el + ",";
    }
    bots = bots.substr(0, bots.length() - 1);
  }

  std::ofstream file;
  file.exceptions(std::ios_base::failbit);
  file.open(path_, std::ios::out);
  file << bots;
}

}  // namespace botscript_server
