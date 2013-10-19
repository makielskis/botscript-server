// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./delete_bot_op.h"

namespace botscript_server {

delete_bot_op::delete_bot_op(const std::string& sid,
                             const std::string& identifier)
    : user_op(sid),
      identifier_(identifier) {
}

delete_bot_op::delete_bot_op(const rapidjson::Document& doc)
    : user_op(doc),
      identifier_(doc["arguments"]["identifier"].GetString()) {
}

const std::string& delete_bot_op::identifier() const {
  return identifier_;
}

std::vector<std::string> delete_bot_op::type() const {
  return { "user", "bot", "delete" };
}

void delete_bot_op::execute(bs_server& server, op_callback cb) const {
}

}  // namespace botscript_server
