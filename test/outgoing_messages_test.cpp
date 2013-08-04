#include "gtest/gtest.h"

#include <vector>
#include <string>

#include "../src/messages/rapidjson_with_exception.h"
#include "../src/messages/outgoing/success_msg.h"
#include "../src/messages/outgoing/failure_msg.h"

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
  ASSERT_STREQ("success", d["type"].GetString());

  ASSERT_TRUE(d.HasMember("arguments"));
  ASSERT_TRUE(d["arguments"].IsObject());
  ASSERT_TRUE(d["arguments"].HasMember("request"));
  ASSERT_TRUE(d["arguments"].HasMember("request_id"));
  ASSERT_TRUE(d["arguments"]["request_id"].IsUint());
  ASSERT_EQ(d["arguments"]["request_id"].GetUint(), 100u);
  ASSERT_TRUE(d["arguments"]["request"].IsArray());
  ASSERT_TRUE(d["arguments"]["request"][0u].IsString());
  ASSERT_TRUE(d["arguments"]["request"][1u].IsString());
  ASSERT_STREQ("a", d["arguments"]["request"][0u].GetString());
  ASSERT_STREQ("b", d["arguments"]["request"][1u].GetString());
}

TEST(outgoing_messages_test, failure_msg_test) {
  std::vector<std::string> request;
  request.emplace_back("a");
  request.emplace_back("b");
  failure_msg msg(100, request, 1, "User already exists");

  rapidjson::Document d;
  ASSERT_FALSE(d.Parse<0>(msg.to_json().c_str()).HasParseError());

  ASSERT_TRUE(d.IsObject());
  ASSERT_TRUE(d.HasMember("type"));
  ASSERT_TRUE(d["type"].IsString());
  ASSERT_STREQ("failure", d["type"].GetString());

  ASSERT_TRUE(d.HasMember("arguments"));
  ASSERT_TRUE(d["arguments"].IsObject());
  ASSERT_TRUE(d["arguments"].HasMember("request"));
  ASSERT_TRUE(d["arguments"]["request"].IsArray());
  ASSERT_TRUE(d["arguments"]["request"][0u].IsString());
  ASSERT_TRUE(d["arguments"]["request"][1u].IsString());
  ASSERT_STREQ("a", d["arguments"]["request"][0u].GetString());
  ASSERT_STREQ("b", d["arguments"]["request"][1u].GetString());
  ASSERT_TRUE(d["arguments"].HasMember("reason"));
  ASSERT_TRUE(d["arguments"]["reason"].IsString());
  ASSERT_STREQ("User already exists", d["arguments"]["reason"].GetString());
  ASSERT_TRUE(d["arguments"].HasMember("error_code"));
  ASSERT_TRUE(d["arguments"]["error_code"].IsUint());
  ASSERT_EQ(1u, d["arguments"]["error_code"].GetUint());
}
