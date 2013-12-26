#include <memory>

#include "gtest/gtest.h"

#include "dust/storage/mem_store.h"
#include "dust/document.h"

#include "../src/user.h"
#include "../src/db_bot_config.h"

using namespace dust;
using namespace botscript_server;

class user_bot_config_test: public testing::Test {
public:
  user_bot_config_test()
      : store_(std::make_shared<mem_store>()),
        users_(store_, "users"),
        user_(users_["test_user"], "password", "mail@foo.bar") {
  }

protected:
  std::shared_ptr<key_value_store> store_;
  document users_;
  user user_;
};

TEST_F(user_bot_config_test, add_bot_config_test) {
  auto config1 =
      std::make_shared<db_bot_config>(
          users_["test_user"]["bots"]["pg_hh_penner"],
          R"({
  "username": "penner",
  "password": "penner_password",
  "package": "pg",
  "server": "http://www.pennergame.de",
  "modules": {
    "base": {
      "proxy": "12.34.56.78:1000",
      "wait_time_factor": "0.50"
    }
  }
})");

  auto config2 =
      std::make_shared<db_bot_config>(
          users_["test_user"]["bots"]["kv_sf_knasti"],
          R"({
  "username": "knasti",
  "password": "knasti_password",
  "package": "kv",
  "server": "http://www.knastvoegel.de",
  "modules": {
    "base": {
      "proxy": "98.76.54.32:1000",
      "wait_time_factor": "2.00"
    }
  }
})");

  EXPECT_EQ(2u, user_.bot_configs().size());

  for (const auto& config : user_.bot_configs()) {
    if (config->identifier() == "pg_hh_penner") {
      EXPECT_EQ("penner_password", config->password());
      EXPECT_EQ("pg", config->package());
      EXPECT_EQ("http://www.pennergame.de", config->server());
      EXPECT_EQ("12.34.56.78:1000", config->module_settings()["base"]["proxy"]);
      EXPECT_EQ("0.50", config->module_settings()["base"]["wait_time_factor"]);
    } else if (config->identifier() == "kv_sf_knasti") {
      EXPECT_EQ("knasti_password", config->password());
      EXPECT_EQ("kv", config->package());
      EXPECT_EQ("http://www.knastvoegel.de", config->server());
      EXPECT_EQ("98.76.54.32:1000", config->module_settings()["base"]["proxy"]);
      EXPECT_EQ("2.00", config->module_settings()["base"]["wait_time_factor"]);
    } else {
      EXPECT_TRUE(false);
    }
  }
}
