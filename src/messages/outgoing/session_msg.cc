// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./session_msg.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace botscript_server {

session_msg::session_msg(const time_t expire, const std::string& sid)
    : expire_(expire),
      sid_(sid) {
}

std::string session_msg::to_json() const {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  {
    writer.StartObject();

    writer.String("type");
    writer.String("session");

    writer.String("arguments");
    {
      writer.StartObject();

      writer.String("sid");
      writer.String(sid_.c_str(), (rapidjson::SizeType) sid_.length());

      writer.String("expire");
      writer.Uint(expire_);

      writer.EndObject();
    }

    writer.EndObject();
  }

  return buffer.GetString();
}

}  // namespace botscript_server
