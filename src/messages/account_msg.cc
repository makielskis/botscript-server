// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./account_msg.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace botscript_server {

account_msg::account_msg(const std::string& email) : email_(email) {}

const std::string& account_msg::email() const { return email_; }

std::string account_msg::to_json() const {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  {
    writer.StartObject();

    writer.String("type");
    writer.String("account");

    writer.String("arguments");
    {
      writer.StartObject();

      writer.String("email");
      writer.String(email_.c_str(), (rapidjson::SizeType)email_.length());

      writer.EndObject();
    }

    writer.EndObject();
  }

  return buffer.GetString();
}

}  // namespace botscript_server
