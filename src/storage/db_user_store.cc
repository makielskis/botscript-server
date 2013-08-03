// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "db_user_store.h"

#include <iostream>
#include <sstream>
#include <ctime>
#include <stdexcept>

#include "boost/random/random_device.hpp"
#include "boost/random/uniform_int_distribution.hpp"
#include "boost/algorithm/string/split.hpp"

#include "websocketpp/common/md5.hpp"

#include "../error.h"

#define SESSION_DURATION 604800

namespace bs = botscript;

namespace botscript_server {

db_user_store::db_user_store(const std::string& path,
                             boost::asio::io_service* io_service)
  : io_service_(io_service),
    db_(path),
    users_(db_.get_entry("users")) {
}

void db_user_store::io_service(boost::asio::io_service* io_service) {
  io_service_ = io_service;
}

void db_user_store::add_user(const std::string& username,
                             const std::string& password,
                             const std::string& email,
                             cb<std::string>::type cb) {
  io_service_->post([=]() {
    if (!users_[username]["password"].exists()) {
      std::string sid = make_session(username);

      users_[username]["password"] = websocketpp::md5::md5_hash_hex(password);
      users_[username]["email"] = email;

      return cb(sid, boost::system::error_code());
    } else {
      return cb("", error::username_already_taken);
    }
  });
}

void db_user_store::remove_user(const std::string& session_id,
                                const std::string& password,
                                empty_cb cb) {
  io_service_->post([=]() {
    if (!check_session(session_id)) {
      return cb(error::session_id_not_available);
    } else {
      const auto it = sid_user_.find(session_id);
      if (it != sid_user_.cend()) {
        const std::string& username = it->second;
        entry user = users_[username];
        if (users_[username]["password"].exists()) {
          if (websocketpp::md5::md5_hash_hex(password) == user["password"].val()) {
            user["password"].remove();
            user["email"].remove();
            user["bots"].remove();

            user_sid_.erase(username);
            sid_expire_.erase(session_id);
            sid_user_.erase(it);

            return cb(boost::system::error_code());
          } else {
            return cb(error::password_wrong);
          }
        } else {
          // This should not happen
          return cb(error::user_not_found);
        }
      } else {
        return cb(error::session_id_not_available);
      }
    }
  });
}

void db_user_store::login(const std::string& username,
                          const std::string& password,
                          cb<std::string>::type cb) {
  io_service_->post([=]() {
    if (users_[username]["password"].exists()) {
      std::string db_pw = users_[username]["password"].val();
      if (websocketpp::md5::md5_hash_hex(password) == db_pw) {
        std::string sid = make_session(username);
        return cb(sid, boost::system::error_code());
      } else {
        return cb("", error::password_wrong);
      }
    } else {
      return cb("", error::user_not_found);
    }
  });
}

void db_user_store::logout(const std::string& session_id, empty_cb cb) {
  const auto it = sid_user_.find(session_id);
  if (it != sid_user_.cend()) {
    const std::string& username = it->second;
    user_sid_.erase(username);
    sid_expire_.erase(session_id);
    sid_user_.erase(it);
    return cb(boost::system::error_code());
  } else {
    return cb(error::session_id_not_available);
  }
}

void db_user_store::set_password(const std::string& session_id,
                                 const std::string& old_password,
                                 const std::string& new_password,
                                 empty_cb cb) {
  io_service_->post([=]() {
    if (!check_session(session_id)) {
      return cb(error::session_id_not_available);
    } else {
      const auto it = sid_user_.find(session_id);
      if (it != sid_user_.cend()) {
        const std::string& username = it->second;
        entry password = users_[username]["password"];
        if (websocketpp::md5::md5_hash_hex(old_password) == password.val()) {
          password = websocketpp::md5::md5_hash_hex(new_password);
          return cb(boost::system::error_code());
        } else {
          return cb(error::password_wrong);
        }
      } else {
        // This should not happen
        return cb(error::user_not_found);
      }
    }
  });
}

void db_user_store::get_email(const std::string& session_id,
                              cb<std::string>::type cb) {
  io_service_->post([=]() {
    if (!check_session(session_id)) {
      return cb("", error::session_id_not_available);
    } else {
      const auto it = sid_user_.find(session_id);
      if (it != sid_user_.cend()) {
        const std::string& username = it->second;
        return cb(users_[username]["email"].val(), boost::system::error_code());
      } else {
        // This should not happen
        return cb("", error::user_not_found);
      }
    }
  });
}

void db_user_store::set_email(const std::string& session_id,
                              const std::string& password,
                              const std::string& email,
                              empty_cb cb) {
  io_service_->post([=]() {
    if (!check_session(session_id)) {
      return cb(error::session_id_not_available);
    } else {
      const auto it = sid_user_.find(session_id);
      if (it != sid_user_.cend()) {
        const std::string& username = it->second;
        std::string db_pw = users_[username]["password"].val();
        if (websocketpp::md5::md5_hash_hex(password) == db_pw) {
          users_[username]["email"] = email;
          return cb(boost::system::error_code());
        } else {
          return cb(error::password_wrong);
        }
      } else {
        // This should not happen
        return cb(error::user_not_found);
      }
    }
  });
}

void db_user_store::get_bots(const std::string& session_id,
                             cb<std::vector<std::string>>::type cb)  {
  io_service_->post([=]() {
    if (!check_session(session_id)) {
      return cb(std::vector<std::string>(),
                error::session_id_not_available);
    } else {
      const auto it = sid_user_.find(session_id);
      if (it != sid_user_.cend()) {
        const std::string& username = it->second;
        std::string bots_string = users_[username]["bots"].val();

        if (bots_string.empty()) {
          return cb(std::vector<std::string>(), boost::system::error_code());
        } else {
          std::vector<std::string> bots;
          boost::split(bots, bots_string, boost::is_any_of(","));

          return cb(bots, boost::system::error_code());
        }
      } else {
        // This should not happen
        return cb(std::vector<std::string>(), error::user_not_found);
      }
    }
  });
}

void db_user_store::add_bot(const std::string& session_id,
                            const std::string& bot_identifier,
                            empty_cb cb) {
  io_service_->post([=]() {
    if (!check_session(session_id)) {
      return cb(error::session_id_not_available);
    } else {
      const auto it = sid_user_.find(session_id);
      if (it != sid_user_.cend()) {
        const std::string& username = it->second;
        entry db_bots = users_[username]["bots"];
        std::string bots = users_[username]["bots"].val();

        if (bots.find(bot_identifier) != std::string::npos) {
          return cb(error::bot_already_exists);
        } else {
          std::stringstream ss;
          ss << bots;
          if (!bots.empty()) {
            ss << ",";
          }
          ss << bot_identifier;
          db_bots = ss.str();
          return cb(boost::system::error_code());
        }
      } else {
        // This should not happen
        return cb(error::user_not_found);
      }
    }
  });
}

void db_user_store::remove_bot(const std::string& session_id,
                               const std::string& identifier,
                               empty_cb cb) {
  io_service_->post([=]() {
    if (!check_session(session_id)) {
      return cb(error::session_id_not_available);
    } else {
      const auto it = sid_user_.find(session_id);
      if (it != sid_user_.cend()) {
        const std::string& username = it->second;
        entry db_bots = users_[username]["bots"];
        std::string bots = db_bots.val();

        auto pos = bots.find(identifier);
        if (pos != std::string::npos) {
          if (pos == 0) {
            bool last = identifier.length() == bots.length();
            bots.erase(0, identifier.length() + (last ? 0 : 1));
          } else {
            bots.erase(pos - 1, identifier.length() + 1);
          }
          db_bots = bots;
          return cb(boost::system::error_code());
        } else {
          return cb(error::bot_not_found);
        }
      } else {
        // This should not happen
        return cb(error::user_not_found);
      }
    }
  });
}

const std::unordered_map<std::string, std::string>& db_user_store::sid_user() {
  return sid_user_;
}

const std::unordered_map<std::string, std::string>& db_user_store::user_sid() {
  return user_sid_;
}

const std::unordered_map<std::string, time_t>& db_user_store::sid_expire() {
  return sid_expire_;
}

std::string db_user_store::make_session(const std::string& username) {
  // Setup random generator
  std::string chars("abcdefghijklmnopqrstuvwxyz"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "1234567890");
  boost::random::random_device rng;
  boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);

  // Generate session id
  std::stringstream ss;
  for(int i = 0; i < 32; ++i) {
    ss << chars[index_dist(rng)];
  }
  std::string sid = ss.str();

  // Clean mappings
  const auto it = user_sid_.find(username);
  if (it != user_sid_.cend()) {
    sid_user_.erase(it->second);
    sid_expire_.erase(it->second);
  }

  // Update mappings
  sid_user_[sid] = username;
  user_sid_[username] = sid;
  sid_expire_[sid] = time(nullptr) + SESSION_DURATION;

  return sid;
}

bool db_user_store::check_session(const std::string& sid) {
  auto it = sid_expire_.find(sid);
  if (it == sid_expire_.end()) {
    return false;
  }

  time_t now = time(nullptr);
  if (it->second < now) {
    return false;
  }

  it->second = now + SESSION_DURATION;
  return true;
}

}  // namespace botscript_server
