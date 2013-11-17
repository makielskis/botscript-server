// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_USER_H_
#define BOTSCRIPT_SERVER_USER_H_

#include <string>
#include <vector>
#include <memory>

#include "bot_config.h"

#include "dust/document.h"

namespace botscript_server {

class user {
 public:
  user(dust::document doc);
  user(dust::document doc,
       const std::string& username,
       const std::string& password);

  std::string username();

  void password(const std::string& old_pw, const std::string& new_pw);

  bool check_password(const std::string& password);

  std::vector<std::shared_ptr<botscript::bot_config>> bots_config();

  dust::document bots_document();

 private:
  dust::document doc_;
};

}  // namespace botscript_server

#endif  // BOTSERVER_USER_H_
