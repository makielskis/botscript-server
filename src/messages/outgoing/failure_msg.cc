// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./failure_msg.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace botscript_server {

failure_msg::failure_msg(unsigned int request_id,
                         const std::vector<std::string>& request,
                         unsigned int error_code,
                         const std::string& reason)
    : request_id_(request_id),
      request_(request),
      error_code_(error_code),
      reason_(reason) {
}

std::string failure_msg::to_json() const {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  {
    writer.StartObject();

    writer.String("type");
    writer.String("failure");

    writer.String("arguments");
    {
      writer.StartObject();

      writer.String("request");
      writer.StartArray();
      for (const std::string& s : request_) {
        writer.String(s.c_str(), (rapidjson::SizeType) s.length());
      }
      writer.EndArray();

      writer.String("request_id");
      writer.Uint(request_id_);

      writer.String("error_code");
      writer.Uint(error_code_);

      writer.String("reason");
      writer.String(reason_.c_str());

      writer.EndObject();
    }

    writer.EndObject();
  }

  return buffer.GetString();
}

}  // namespace botscript_server
