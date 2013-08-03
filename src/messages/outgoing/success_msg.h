// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_OUTGOING_SUCCESS_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_OUTGOING_SUCCESS_MSG_

#include <string>
#include <vector>

#include "./outgoing_msg.h"

namespace botscript_server {

class success_msg : public outgoing_msg {
 public:
  success_msg(unsigned int request_id,
              const std::vector<std::string>& request);

  virtual std::string to_json() const;

 private:
  const unsigned int request_id_;
  const std::vector<std::string> request_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_OUTGOING_SUCCESS_MSG_
