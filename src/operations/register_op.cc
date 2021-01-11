// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./register_op.h"

#include <fstream>
#include <istream>

#include "../allowed_users_parser.h"
#include "../bs_server.h"
#include "../error.h"
#include "../make_unique.h"
#include "../messages/account_msg.h"
#include "../messages/bots_msg.h"
#include "../messages/packages_msg.h"
#include "../messages/session_msg.h"
#include "../user.h"

namespace botscript_server {

register_op::register_op(const rapidjson::Document& doc) {
  username_ = doc["arguments"]["username"].GetString();
  password_ = doc["arguments"]["password"].GetString();
  email_ = doc["arguments"]["email"].GetString();
}

register_op::register_op(const std::string& username,
                         const std::string& password, const std::string& email)
    : username_(username), password_(password), email_(email) {}

const std::string& register_op::username() const { return username_; }

const std::string& register_op::password() const { return password_; }

const std::string& register_op::email() const { return email_; }

std::vector<std::string> register_op::type() const { return {"register"}; }

std::vector<msg_ptr> register_op::execute(bs_server& server,
                                          op_callback cb) const {
  check_user_allowed(server.options_, username());

  user u(server.users_[username()], password(), email());

  server.update_session(u);
  cb(u.session_id(), std::vector<msg_ptr>());

  std::vector<msg_ptr> out;
  out.emplace_back(
      make_unique<session_msg>(u.session_expire(), u.session_id()));
  out.emplace_back(make_unique<account_msg>(u.email()));
  out.emplace_back(make_unique<packages_msg>(server.packages_));
  out.emplace_back(make_unique<bots_msg>(std::map<std::string, std::string>()));
  return out;
}

}  // namespace botscript_server
