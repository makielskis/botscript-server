// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./success_msg.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace botscript_server {

success_msg::success_msg(unsigned int request_id,
                         const std::vector<std::string>& request)
    : request_id_(request_id), request_(request) {}

std::string success_msg::to_json() const {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  {
    writer.StartObject();

    writer.String("type");
    writer.String("success");

    writer.String("arguments");
    {
      writer.StartObject();

      writer.String("request");
      writer.StartArray();
      for (const std::string& s : request_) {
        writer.String(s.c_str(), (rapidjson::SizeType)s.length());
      }
      writer.EndArray();

      writer.String("request_id");
      writer.Uint(request_id_);

      writer.EndObject();
    }

    writer.EndObject();
  }

  return buffer.GetString();
}

}  // namespace botscript_server
