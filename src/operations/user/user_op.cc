// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./user_op.h"

#include "bot.h"
#include "bot_config.h"

#include "../../bs_server.h"
#include "../../error.h"
#include "../../make_unique.h"
#include "../../messages/account_msg.h"
#include "../../messages/bots_msg.h"
#include "../../messages/packages_msg.h"
#include "../../messages/session_msg.h"
#include "../../messages/update_msg.h"
#include "../../user.h"
#include "./bot/bot_util.h"

namespace botscript_server {

user_op::user_op(const std::string& sid) : sid_(sid) {}

user_op::user_op(const rapidjson::Document& doc) {
  sid_ = doc["arguments"]["sid"].GetString();
}

const std::string& user_op::sid() const { return sid_; }

std::vector<std::string> user_op::type() const { return {"user"}; }

std::vector<msg_ptr> user_op::execute(bs_server& server, op_callback cb) const {
  user u = get_user_from_session(server);
  cb(u.session_id(), std::vector<msg_ptr>());

  auto bot_configurations = bot_configs(u, server);

  std::vector<msg_ptr> out;
  out.emplace_back(
      make_unique<session_msg>(u.session_expire(), u.session_id()));
  out.emplace_back(make_unique<account_msg>(u.email()));
  out.emplace_back(make_unique<packages_msg>(server.packages_));
  out.emplace_back(make_unique<bots_msg>(bot_configurations));
  for (auto const& entry : server.bot_logs(u)) {
    out.emplace_back(make_unique<update_msg>(entry.first, "log", entry.second));
  }

  for (auto const& bot_config : bot_configurations) {
    try {
      auto updates = get_bot(u, server, bot_config.first)->update_all_shared();
      for (auto const& upd : updates) {
        out.emplace_back(
            make_unique<update_msg>(bot_config.first, upd.first, upd.second));
      }
    } catch (boost::system::system_error const&) {
      continue;
    }
  }

  return out;
}

user user_op::get_user_from_session(bs_server& server) const {
  if (server.options_.autologin) {
    auto guest_doc = server.users_["guest"];

    if (!guest_doc.exists()) {
      user guest(guest_doc, "password", "guest@localhost.tld");
      guest.new_session();
    }

    user guest = user(guest_doc);
    server.update_session(guest);
    return guest;
  } else {
    typedef session_set::index<index_session_id>::type sid_index;

    auto& sessions_indexed_by_sid = server.sessions_.get<index_session_id>();
    sid_index::iterator it = sessions_indexed_by_sid.find(sid());

    if (it == sessions_indexed_by_sid.end()) {
      throw boost::system::system_error(error::session_id_not_available);
    }

    if (!it->u.session_active()) {
      throw boost::system::system_error(error::session_id_timed_out);
    }

    server.update_session(it->u);

    return it->u;
  }
}

}  // namespace botscript_server
