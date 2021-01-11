#include "gtest/gtest.h"

#include <map>
#include <vector>
#include <string>

#include "rapidjson/document.h"

#include "../src/messages/success_msg.h"
#include "../src/messages/failure_msg.h"
#include "../src/messages/packages_msg.h"
#include "../src/messages/session_msg.h"
#include "../src/messages/account_msg.h"
#include "../src/messages/bots_msg.h"
#include "../src/messages/update_msg.h"

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

TEST(outgoing_messages_test, packages_msg_test) {
  std::vector<std::string> packages;
  packages.push_back("{ \"a\": \"b\" }");
  packages.push_back("{ \"c\": \"d\" }");

  packages_msg msg(packages);

  rapidjson::Document d;
  ASSERT_FALSE(d.Parse<0>(msg.to_json().c_str()).HasParseError());

  ASSERT_TRUE(d.IsObject());
  ASSERT_TRUE(d.HasMember("type"));
  ASSERT_TRUE(d["type"].IsString());
  ASSERT_STREQ("packages", d["type"].GetString());

  ASSERT_TRUE(d.HasMember("arguments"));
  ASSERT_TRUE(d["arguments"].IsArray());
  ASSERT_TRUE(d["arguments"][0u].IsObject());
  ASSERT_TRUE(d["arguments"][1u].IsObject());
  ASSERT_TRUE(d["arguments"][0u].HasMember("a"));
  ASSERT_TRUE(d["arguments"][1u].HasMember("c"));
  ASSERT_STREQ("b", d["arguments"][0u]["a"].GetString());
  ASSERT_STREQ("d", d["arguments"][1u]["c"].GetString());
}

TEST(outgoing_messages_test, session_msg_test) {
  session_msg msg(123u, "random session id");

  rapidjson::Document d;
  ASSERT_FALSE(d.Parse<0>(msg.to_json().c_str()).HasParseError());

  ASSERT_TRUE(d.IsObject());
  ASSERT_TRUE(d.HasMember("type"));
  ASSERT_TRUE(d["type"].IsString());
  ASSERT_STREQ("session", d["type"].GetString());

  ASSERT_TRUE(d.HasMember("arguments"));
  ASSERT_TRUE(d["arguments"].IsObject());
  ASSERT_TRUE(d["arguments"].HasMember("sid"));
  ASSERT_TRUE(d["arguments"]["sid"].IsString());
  ASSERT_STREQ("random session id", d["arguments"]["sid"].GetString());
  ASSERT_TRUE(d["arguments"].HasMember("expire"));
  ASSERT_TRUE(d["arguments"]["expire"].IsUint());
  ASSERT_EQ(123u, d["arguments"]["expire"].GetUint());
}

TEST(outgoing_messages_test, account_msg_test) {
  account_msg msg("a@b.cd");

  rapidjson::Document d;
  ASSERT_FALSE(d.Parse<0>(msg.to_json().c_str()).HasParseError());

  ASSERT_TRUE(d.IsObject());
  ASSERT_TRUE(d.HasMember("type"));
  ASSERT_TRUE(d["type"].IsString());
  ASSERT_STREQ("account", d["type"].GetString());

  ASSERT_TRUE(d.HasMember("arguments"));
  ASSERT_TRUE(d["arguments"].IsObject());
  ASSERT_TRUE(d["arguments"].HasMember("email"));
  ASSERT_TRUE(d["arguments"]["email"].IsString());
  ASSERT_STREQ("a@b.cd", d["arguments"]["email"].GetString());
}

TEST(outgoing_messages_test, empty_bots_msg_test) {
  std::map<std::string, std::string> bots;

  bots_msg msg(bots);

  rapidjson::Document d;
  ASSERT_FALSE(d.Parse<0>(msg.to_json().c_str()).HasParseError());

  ASSERT_TRUE(d.IsObject());
  ASSERT_TRUE(d.HasMember("type"));
  ASSERT_TRUE(d["type"].IsString());
  ASSERT_STREQ("bots", d["type"].GetString());

  ASSERT_TRUE(d.HasMember("arguments"));
  ASSERT_TRUE(d["arguments"].IsObject());
}

TEST(outgoing_messages_test, bots_msg_test) {
  std::map<std::string, std::string> bots;
  bots["x"] = "{ \"a\": \"b\" }";
  bots["y"] = "{ \"c\": \"d\" }";

  bots_msg msg(bots);

  rapidjson::Document d;
  ASSERT_FALSE(d.Parse<0>(msg.to_json().c_str()).HasParseError());

  ASSERT_TRUE(d.IsObject());
  ASSERT_TRUE(d.HasMember("type"));
  ASSERT_TRUE(d["type"].IsString());
  ASSERT_STREQ("bots", d["type"].GetString());

  ASSERT_TRUE(d.HasMember("arguments"));
  ASSERT_TRUE(d["arguments"].IsObject());
  ASSERT_TRUE(d["arguments"].HasMember("x"));
  ASSERT_TRUE(d["arguments"]["x"].IsObject());
  ASSERT_TRUE(d["arguments"]["x"].HasMember("a"));
  ASSERT_TRUE(d["arguments"]["x"]["a"].IsString());
  ASSERT_STREQ("b", d["arguments"]["x"]["a"].GetString());
  ASSERT_TRUE(d["arguments"].HasMember("y"));
  ASSERT_TRUE(d["arguments"]["y"].IsObject());
  ASSERT_TRUE(d["arguments"]["y"].HasMember("c"));
  ASSERT_TRUE(d["arguments"]["y"]["c"].IsString());
  ASSERT_STREQ("d", d["arguments"]["y"]["c"].GetString());
}

TEST(outgoing_messages_test, update_msg_test) {
  update_msg msg("pg_hh_test", "key1", "value1");

  rapidjson::Document d;
  ASSERT_FALSE(d.Parse<0>(msg.to_json().c_str()).HasParseError());

  ASSERT_TRUE(d.IsObject());
  ASSERT_TRUE(d.HasMember("type"));
  ASSERT_TRUE(d["type"].IsString());
  ASSERT_STREQ("update", d["type"].GetString());

  ASSERT_TRUE(d.HasMember("arguments"));
  ASSERT_TRUE(d["arguments"].IsObject());
  ASSERT_TRUE(d["arguments"].HasMember("identifier"));
  ASSERT_TRUE(d["arguments"]["identifier"].IsString());
  ASSERT_STREQ("pg_hh_test", d["arguments"]["identifier"].GetString());
  ASSERT_TRUE(d["arguments"].HasMember("key"));
  ASSERT_TRUE(d["arguments"]["key"].IsString());
  ASSERT_STREQ("key1", d["arguments"]["key"].GetString());
  ASSERT_TRUE(d["arguments"].HasMember("value"));
  ASSERT_TRUE(d["arguments"]["value"].IsString());
  ASSERT_STREQ("value1", d["arguments"]["value"].GetString());
}
