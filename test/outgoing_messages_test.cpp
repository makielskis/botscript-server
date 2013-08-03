#include "gtest/gtest.h"

#include <vector>
#include <string>

#include "../src/messages/rapidjson_with_exception.h"
#include "../src/messages/outgoing/success_msg.h"

using namespace botscript_server;

TEST(outgoing_messages_test, success_msg_test) {
  std::vector<std::string> request;
  request.emplace_back("a");
  request.emplace_back("b");
  success_msg msg(100, request);

  rapidjson::Document d;
  ASSERT_FALSE(d.Parse<0>(msg.to_json().c_str()).HasParseError());

  ASSERT_TRUE(d.IsObject());
  ASSERT_TRUE(d.HasMember("type"));
  ASSERT_TRUE(d["type"].IsString());
  ASSERT_STREQ(d["type"].GetString(), "success");
}
