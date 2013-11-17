#include "gtest/gtest.h"

#include "bot_config.h"
#include "mem_bot_config.h"
#include "../src/db_bot_config.h"

#include "dust/storage/mem_store.h"

#define TEST_JSON "{"\
  "\"username\":\"test_name\","\
  "\"password\":\"test_password\","\
  "\"package\":\"test_package\","\
  "\"server\":\"test_server\","\
  "\"modules\":{"\
    "\"base\":{"\
      "\"wait_time_factor\": \"1.00\","\
      "\"proxy\": \"\""\
    "},"\
    "\"collect\":{\"active\":\"1\"},"\
    "\"sell\":{\"price\":\"20\",\"active\":\"0\"}"\
  "}"\
"}"

using namespace botscript;
using namespace botscript_server;
using namespace dust;

class bot_config_test : public testing::Test {
 protected:
  bot_config_test()
    : store_(std::make_shared<mem_store>()),
      doc_(document(store_, "users")["foo"]["bots"]["pg_hh_bot"]),
      config_(std::make_shared<db_bot_config>(doc_, TEST_JSON)) {
  }

  std::shared_ptr<key_value_store> store_;
  document doc_;
  std::shared_ptr<bot_config> config_;
};

TEST_F(bot_config_test, invalid_json_test) {
  EXPECT_THROW({
    std::make_shared<db_bot_config>(doc_, "{ . }");
  }, std::runtime_error);
}

TEST_F(bot_config_test, invalid_config_test) {
  EXPECT_THROW({
    std::make_shared<db_bot_config>(doc_, "\"test\"");
  }, std::runtime_error);
}

TEST_F(bot_config_test, username_test) {
  EXPECT_EQ(this->config_->username(), "test_name");
}

TEST_F(bot_config_test, password_test) {
  EXPECT_EQ(this->config_->password(), "test_password");
}

TEST_F(bot_config_test, package_test) {
  EXPECT_EQ(this->config_->package(), "test_package");
}

TEST_F(bot_config_test, server_test) {
  EXPECT_EQ(this->config_->server(), "test_server");
}

TEST_F(bot_config_test, module_config_test) {
  auto modules = this->config_->module_settings();
  ASSERT_FALSE(modules.find("base") == modules.end());
  ASSERT_FALSE(modules.find("base")->second.find("proxy") == modules.find("base")->second.end());
  EXPECT_EQ(this->config_->module_settings()["base"]["wait_time_factor"], "1.00");
  EXPECT_EQ(this->config_->module_settings()["base"]["proxy"], "");
}
