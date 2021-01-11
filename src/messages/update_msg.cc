// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./update_msg.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace botscript_server {

update_msg::update_msg(const std::string& id, const std::string& k,
                       const std::string& v)
    : id_(id), k_(k), v_(v) {}

std::string update_msg::to_json() const {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  {
    writer.StartObject();

    writer.String("type");
    writer.String("update");

    writer.String("arguments");
    {
      writer.StartObject();

      writer.String("identifier");
      writer.String(id_.c_str(), (rapidjson::SizeType)id_.size());

      writer.String("key");
      writer.String(k_.c_str(), (rapidjson::SizeType)k_.size());

      writer.String("value");
      writer.String(v_.c_str(), (rapidjson::SizeType)v_.size());

      writer.EndObject();
    }

    writer.EndObject();
  }

  return buffer.GetString();
}

}  // namespace botscript_server
