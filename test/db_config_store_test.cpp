#include "gtest/gtest.h"

#include <string>
#include <vector>
#include <memory>

#include "boost/asio/io_service.hpp"
#include "boost/filesystem.hpp"

#include "bot.h"
#include "../src/storage/db_config_store.h"
#include "../src/storage/kc_wrapper.h"


#define DB_FILE           "./config_test.kch"
#define INPUT_CONFIG      "{\"username\":\"login_username\",\"password\":\"login_password\",\"package\":\"ts\",\"server\":\"http://example.com\",\"inactive\":false,\"modules\":{\"base\":{\"wait_time_factor\":\"1.00\",\"proxy\":\"\"}}}"
#define INPUT_CONFIG_1    "{\"username\":\"login_username1\",\"password\":\"login_password\",\"package\":\"ts\",\"server\":\"http://example.com\",\"inactive\":false,\"modules\":{\"base\":{\"wait_time_factor\":\"1.00\",\"proxy\":\"\"}}}"
#define EXPECTED_CONFIG   "{\"username\":\"login_username\",\"password\":\"login_password\",\"package\":\"ts\",\"server\":\"http://example.com\",\"inactive\":false,\"modules\":{\"base\":{\"proxy\":\"\",\"wait_time_factor\":\"1.00\"},\"crime\":{\"active\":\"0\",\"crime\":\"\",\"crime_from\":\"\"}}}"
#define EXPECTED_CONFIG_1 "{\"username\":\"login_username1\",\"password\":\"login_password\",\"package\":\"ts\",\"server\":\"http://example.com\",\"inactive\":false,\"modules\":{\"base\":{\"proxy\":\"\",\"wait_time_factor\":\"1.00\"},\"crime\":{\"active\":\"0\",\"crime\":\"\",\"crime_from\":\"\"}}}"
#define IDENTIFIER        "ts_ex_login_username"
#define IDENTIFIER_1      "ts_ex_login_username1"

using namespace std;
using namespace botscript;
using namespace botscript_server;
using boost::system::error_code;

class DbConfigStoreTest : public testing::Test {
 protected:
  virtual void SetUp() override {
    bot::load_packages("../test/packages");
    addTestConfig(INPUT_CONFIG, EXPECTED_CONFIG);
    addTestConfig(INPUT_CONFIG_1, EXPECTED_CONFIG_1);
  }

  virtual void TearDown() override {
    for (auto b : bots_) {
      b->shutdown();
    }
    boost::filesystem::remove(DB_FILE);
  }

  void addTestConfig(const std::string& config, const std::string& expected) {
    db_config_store store(DB_FILE, &io_service_);

    auto b = make_shared<bot>(&io_service_);
    b->callback_ = [](std::string, std::string k, std::string v) { };
    bots_.push_back(b);

    b->init(config, [&store, &expected](shared_ptr<bot> bot, string init_e) {
      ASSERT_EQ("", init_e);
      store.add(bot, [&store, &expected](error_code add_e) {
        ASSERT_FALSE(add_e);

        vector<std::string> configs = store.get_all();
        ASSERT_NE(std::find(configs.begin(), configs.end(), expected), configs.end());
      });
    });

    io_service_.run();
    io_service_.reset();
  }

  vector<shared_ptr<bot>> bots_;
  boost::asio::io_service io_service_;
};

/*
 * ### Remove config ###
 */
TEST_F(DbConfigStoreTest, RemoveConfig) {
  db_config_store store(DB_FILE, &io_service_);

  vector<std::string> configs = store.get_all();
  ASSERT_EQ(2u, configs.size());

  store.remove(IDENTIFIER, [&store](error_code e) {
    ASSERT_FALSE(e);

    vector<std::string> configs = store.get_all();
    ASSERT_EQ(1u, configs.size());
  });

  io_service_.run();
}

/*
 * ### Get single config ###
 */
TEST_F(DbConfigStoreTest, GetSingleConfig) {
  db_config_store store(DB_FILE, &io_service_);

  store.get(IDENTIFIER, [&store](string config, error_code e) {
    ASSERT_FALSE(e);

    EXPECT_EQ(EXPECTED_CONFIG, config);
  });

  io_service_.run();
}

/*
 * ### Get all bot configurations ###
 */
TEST_F(DbConfigStoreTest, GetMultipleConfig) {
  db_config_store store(DB_FILE, &io_service_);
  std::vector<std::string> identifiers;

  identifiers.emplace_back(IDENTIFIER);
  identifiers.emplace_back(IDENTIFIER_1);
  store.get(identifiers, [&store](vector<string> configs, error_code e) {
    ASSERT_FALSE(e);
    ASSERT_EQ(2u, configs.size());

    EXPECT_EQ(EXPECTED_CONFIG, configs[0]);
    EXPECT_EQ(EXPECTED_CONFIG_1, configs[1]);
  });

  io_service_.run();
}

/*
 * ### Update attribute ###
 */
TEST_F(DbConfigStoreTest, UpdateAttribute) {
  db_config_store store(DB_FILE, &io_service_);

  store.update_attribute(bots_[0], "crime", "crime", "testcrime", [&store](error_code e) {
    ASSERT_FALSE(e);

    store.get(IDENTIFIER, [&store](string config, error_code e) {
      ASSERT_FALSE(e);
      EXPECT_TRUE(config.find("\"crime\":\"testcrime\"") != config.npos);
    });
  });

  io_service_.run();
}

/*
 * ### Double add ###
 */
TEST_F(DbConfigStoreTest, DoubleAdd) {
  db_config_store store(DB_FILE, &io_service_);

  store.add(bots_[0], [&store](error_code e) {
    ASSERT_FALSE(e);

    vector<std::string> configs = store.get_all();
    ASSERT_EQ(2u, configs.size());
    EXPECT_EQ(EXPECTED_CONFIG, configs.at(0));
  });

  io_service_.run();
}

/*
 * ### Remove inexistent ###
 */
TEST_F(DbConfigStoreTest, RemoveNonexisting) {
  db_config_store store(DB_FILE, &io_service_);

  store.remove("not_in_db", [&store](error_code e) {
    ASSERT_FALSE(e);

    vector<std::string> configs = store.get_all();
    ASSERT_EQ(2u, configs.size());
  });

  io_service_.run();
}

/*
 * ### Inactive flag read ###
 */
TEST_F(DbConfigStoreTest, InactiveFlagByDefault) {
  db_config_store store(DB_FILE, &io_service_);

  store.get_inactive(IDENTIFIER, [&store](bool inactive, error_code e) {
    ASSERT_FALSE(e);
    ASSERT_FALSE(inactive);
  });

  io_service_.run();
}

/*
 * ### Inactive flag set ###
 */
TEST_F(DbConfigStoreTest, InactiveFlagSetToTrue) {
  db_config_store store(DB_FILE, &io_service_);

  store.get_inactive(IDENTIFIER, [&store](bool inactive, error_code e) {
    ASSERT_FALSE(e);
    ASSERT_FALSE(inactive);

    store.set_inactive(IDENTIFIER, true, [&store](error_code e) {
      store.get_inactive(IDENTIFIER, [&store](bool inactive, error_code e) {
        ASSERT_FALSE(e);
        ASSERT_TRUE(inactive);
      });
    });
  });

  io_service_.run();
}

TEST_F(DbConfigStoreTest, InactiveFlagSetToFalse) {
  db_config_store store(DB_FILE, &io_service_);

  store.get_inactive(IDENTIFIER, [&store](bool inactive, error_code e) {
    ASSERT_FALSE(e);
    ASSERT_FALSE(inactive);

    store.set_inactive(IDENTIFIER, false, [&store](error_code e) {
      store.get_inactive(IDENTIFIER, [&store](bool inactive, error_code e) {
        ASSERT_FALSE(e);
        ASSERT_FALSE(inactive);
      });
    });
  });

  io_service_.run();
}
