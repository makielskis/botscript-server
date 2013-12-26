// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_CREATE_BOT_OP_
#define BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_CREATE_BOT_OP_

#include <memory>
#include <vector>
#include <string>

#include "rapidjson_with_exception.h"

#include "../user_op.h"

namespace botscript {
class bot;
class bot_config;
}

namespace botscript_server {

class user;

/// Abstract parent for bot creation operations.
///
/// Creates a bot on execution.
///
/// Requires the bot_config() method to be overriden to get the
/// actual bot configuration to load.
class create_bot_op: public user_op {
 public:
  explicit create_bot_op(const std::string& sid);
  create_bot_op(const rapidjson::Document& doc);

  virtual ~create_bot_op();

  virtual std::vector<msg_ptr> execute(bs_server& server,
                                       op_callback cb) const override;

 protected:
  /// \return returns the bot configuration
  virtual std::shared_ptr<botscript::bot_config> bot_config(
      const bs_server& server,
      const user& u) const = 0;

  virtual void on_load_fail(
      std::shared_ptr<botscript::bot_config> config,
      user u) const = 0;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_OPERATIONS_USER_BOT_CREATE_BOT_OP_
