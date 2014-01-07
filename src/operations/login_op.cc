// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./login_op.h"

#include "bot_config.h"

#include "../make_unique.h"
#include "../error.h"
#include "../user.h"
#include "../messages/update_msg.h"
#include "../messages/session_msg.h"
#include "../messages/account_msg.h"
#include "../messages/packages_msg.h"
#include "../messages/bots_msg.h"
#include "../bs_server.h"

namespace botscript_server {

login_op::login_op(const rapidjson::Document& doc) {
  username_ = doc["arguments"]["username"].GetString();
  password_ = doc["arguments"]["password"].GetString();
}

login_op::login_op(const std::string& username, const std::string& password)
    : username_(username),
      password_(password) {
}

const std::string& login_op::username() const {
  return username_;
}

const std::string& login_op::password() const {
  return password_;
}

std::vector<std::string> login_op::type() const {
  return {"login"};
}

std::vector<msg_ptr> login_op::execute(bs_server& server,
                                       op_callback /* cb */) const {
  user u(server.users_[username()]);

  if (!u.check_password(password())) {
    throw boost::system::system_error(error::password_wrong);
  }

  u.new_session();
  server.update_session(u);

  std::map<std::string, std::string> bots;
  for (const auto& config : u.bot_configs()) {
    const auto& blocklist = server.bot_creation_blocklist_;
    if (blocklist.find(config->identifier()) == blocklist.end()) {
      bots[config->identifier()] = config->to_json(false);
    }
  }

  std::vector<msg_ptr> out;
  out.emplace_back(make_unique<session_msg>(u.session_expire(),
                                            u.session_id()));
  out.emplace_back(make_unique<account_msg>(u.email()));
  out.emplace_back(make_unique<packages_msg>(server.packages_));
  out.emplace_back(make_unique<bots_msg>(bots));
  for (const auto& entry : server.bot_logs(u)) {
    out.emplace_back(make_unique<update_msg>(entry.first, "log", entry.second));
  }

  return out;
}

}  // namespace botscript_server
