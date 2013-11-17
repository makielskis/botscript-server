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
      "\"wait_time_factor\":\"1.00\","\
      "\"proxy\":\"\""\
    "},"\
    "\"collect\":{"\
      "\"active\":\"1\""\
    "},"\
    "\"sell\":{"\
      "\"price\":\"20\","\
      "\"active\":\"0\""\
    "}"\
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

TEST_F(bot_config_test, basic_settings_test) {
  EXPECT_EQ(this->config_->username(), "test_name");
  EXPECT_EQ(this->config_->password(), "test_password");
  EXPECT_EQ(this->config_->package(), "test_package");
  EXPECT_EQ(this->config_->server(), "test_server");
}

TEST_F(bot_config_test, module_config_test) {
  auto modules = this->config_->module_settings();

  ASSERT_FALSE(modules.find("base") == modules.end());
  ASSERT_FALSE(modules.find("base")->second.find("proxy") == modules.find("base")->second.end());
  EXPECT_EQ(this->config_->module_settings()["base"]["wait_time_factor"], "1.00");
  EXPECT_EQ(this->config_->module_settings()["base"]["proxy"], "");
}

TEST_F(bot_config_test, command_sequence_test) {
  auto cmds = this->config_->init_command_sequence();

  EXPECT_EQ(4u, cmds.size());
  EXPECT_EQ("base_set_wait_time_factor", cmds[0].first);
  EXPECT_EQ("1.00", cmds[0].second);
  EXPECT_EQ("collect_set_active", cmds[1].first);
  EXPECT_EQ("1", cmds[1].second);
  EXPECT_EQ("sell_set_price", cmds[2].first);
  EXPECT_EQ("20", cmds[2].second);
  EXPECT_EQ("sell_set_active", cmds[3].first);
  EXPECT_EQ("0", cmds[3].second);
}

TEST_F(bot_config_test, inactive_default_val_test) {
  EXPECT_FALSE(this->config_->inactive());
}

TEST_F(bot_config_test, set_inactive_true_test) {
  EXPECT_FALSE(this->config_->inactive());
  this->config_->inactive(true);
  EXPECT_TRUE(this->config_->inactive());
  this->config_->inactive(false);
  EXPECT_FALSE(this->config_->inactive());
}

TEST_F(bot_config_test, set_get_test1) {
  this->config_->set("fake_key", "value");
  this->config_->set("sell_price", "21");

  EXPECT_EQ(this->config_->module_settings()["fake"]["key"], "value");
  EXPECT_EQ(this->config_->module_settings()["sell"]["price"], "21");
  EXPECT_EQ(this->config_->value_of("fake_key"), "value");
  EXPECT_EQ(this->config_->value_of("sell_price"), "21");
}

TEST_F(bot_config_test, set_get_test2) {
  this->config_->set("fake", "key", "value");
  this->config_->set("sell", "price", "21");

  EXPECT_EQ(this->config_->module_settings()["fake"]["key"], "value");
  EXPECT_EQ(this->config_->module_settings()["sell"]["price"], "21");
  EXPECT_EQ(this->config_->value_of("fake_key"), "value");
  EXPECT_EQ(this->config_->value_of("sell_price"), "21");
}

TEST_F(bot_config_test, json_test) {
  EXPECT_EQ(TEST_JSON, this->config_->to_json(true));
  EXPECT_NE(TEST_JSON, this->config_->to_json(false));
  EXPECT_TRUE(this->config_->to_json(false).find("password") == std::string::npos);
}
