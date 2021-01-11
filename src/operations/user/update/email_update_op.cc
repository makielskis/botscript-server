// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./email_update_op.h"

#include "../../../make_unique.h"
#include "../../../messages/success_msg.h"
#include "../../../user.h"

namespace botscript_server {

email_update_op::email_update_op(const rapidjson::Document& doc)
    : user_op(doc),
      current_pw_(doc["arguments"]["current_pw"].GetString()),
      new_email_(doc["arguments"]["new_email"].GetString()) {}

const std::string& email_update_op::current_pw() const { return current_pw_; }

const std::string& email_update_op::new_email() const { return new_email_; }

std::vector<std::string> email_update_op::type() const {
  return {"user", "update", "email"};
}

std::vector<msg_ptr> email_update_op::execute(bs_server& server,
                                              op_callback /* cb */) const {
  user u = get_user_from_session(server);

  u.email(current_pw(), new_email());

  std::vector<msg_ptr> out;
  out.emplace_back(make_unique<success_msg>(0, type()));
  return out;
}

}  // namespace botscript_server
