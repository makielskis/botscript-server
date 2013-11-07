// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DESKTOP_USER_STORE_H_
#define DESKTOP_USER_STORE_H_

#include "user_store.h"

#include <string>
#include <set>

#include "boost/asio/io_service.hpp"

#include "./callback.h"

namespace botscript_server {

class desktop_user_store : public user_store {
 public:
  desktop_user_store(const std::string& path);

  virtual void add_user(const std::string& username,
                        const std::string& password,
                        const std::string& email,
                        cb<std::string>::type cb) override;

  virtual void remove_user(const std::string& session_id,
                           const std::string& password,
                           empty_cb cb) override;

  virtual void login(const std::string& username,
                     const std::string& password,
                     cb<std::string>::type cb) override;

  virtual void logout(const std::string& session_id,
                      empty_cb cb) override;

  virtual void set_password(const std::string& session_id,
                            const std::string& old_password,
                            const std::string& new_password,
                            empty_cb cb) override;

  virtual void get_email(const std::string& session_id,
                         cb<std::string>::type cb) override;

  virtual void set_email(const std::string& session_id,
                         const std::string& password,
                         const std::string& email,
                         empty_cb cb) override;

  virtual void get_bots(const std::string& session_id,
                        cb<std::vector<std::string>>::type cb) override;

  virtual void add_bot(const std::string& session_id,
                       const std::string& identifier,
                       empty_cb cb) override;

  virtual void remove_bot(const std::string& session_id,
                          const std::string& identifier,
                          empty_cb cb) override;

 private:
  void read_bots();
  void write_bots();

  std::string path_;
  std::set<std::string> bots_;
};

}  // namespace botscript_server

#endif  // DESKTOP_USER_STORE_H_
