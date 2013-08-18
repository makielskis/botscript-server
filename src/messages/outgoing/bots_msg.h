// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_OUTGOING_bots_msg_
#define BOTSCRIPT_SERVER_MESSAGES_OUTGOING_bots_msg_

#include <string>
#include <map>

#include "./outgoing_msg.h"

namespace botscript_server {

/// Packages message:
/// "These are your bots"
///
/// {
///   'type': 'bots',
///   'arguments': {
///     [String]: [Object],
///     ...
///   }
/// }
class bots_msg : public outgoing_msg {
 public:
  /// \param  map from bot identifier to the configuration of this bot
  bots_msg(const std::map<std::string, std::string>& bot_configs);

  /// \return the stored bot configurations
  const std::map<std::string, std::string>& bot_configs() const;

  virtual std::string to_json() const override;

 private:
  /// Map from bot identifier to the configuration of this bot
  const std::map<std::string, std::string> bot_configs_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_OUTGOING_bots_msg_
