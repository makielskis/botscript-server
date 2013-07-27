#include "gtest/gtest.h"

#include <string>
#include <vector>
#include <memory>

#include "boost/asio/io_service.hpp"
#include "boost/filesystem.hpp"

#include "bot.h"
#include "../src/storage/db_config_store.h"
#include "../src/storage/kc_wrapper.h"


#define DB_FILE          "./config_test.kch"
#define INPUT_CONFIG     "{\"username\":\"login_username\",\"password\":\"login_password\",\"package\":\"ts\",\"server\":\"http://example.com\",\"modules\":{\"base\":{\"wait_time_factor\":\"1.00\",\"proxy\":\"\"}}}"
#define EXPECTED_CONFIG  "{\"username\":\"login_username\",\"password\":\"login_password\",\"package\":\"ts\",\"server\":\"http://example.com\",\"modules\":{\"base\":{\"proxy\":\"\",\"wait_time_factor\":\"1.00\"},\"crime\":{\"active\":\"0\",\"crime\":\"\",\"crime_from\":\"\"}}}"
#define IDENTIFIER       "ts_ex_login_username"

using namespace std;
using namespace botscript;
using namespace botscript_server;

class DbConfigStoreTest : public testing::Test {
 protected:
  virtual void SetUp() override {
    addTestConfig();
  }

  virtual void TearDown() override {
    testbot_->shutdown();
    boost::filesystem::remove(DB_FILE);
  }

  void addTestConfig() {
    db_config_store store(DB_FILE, &io_service_);

    bot::load_packages("../test/packages");
    testbot_ = make_shared<bot>(&io_service_);
    testbot_->callback_ = [](std::string, std::string k, std::string v) {
    //  cout << k << " -> " << v << "\n";
    };

    testbot_->init(INPUT_CONFIG, [&store](shared_ptr<bot> bot, string init_e){
    ASSERT_EQ("", init_e);
      store.add(bot, [&store](error_indicator add_e) {
        ASSERT_FALSE(add_e);

        vector<std::string> configs = store.get_all();
        ASSERT_EQ(1, configs.size());
        EXPECT_EQ(EXPECTED_CONFIG, configs.at(0));
      });
    });

    io_service_.run();
    io_service_.reset();
  }

  shared_ptr<bot> testbot_;
  boost::asio::io_service io_service_;
};

/*
 * ### Remove config ###
 */
TEST_F(DbConfigStoreTest, RemoveConfig) {
  db_config_store store(DB_FILE, &io_service_);

  store.remove(IDENTIFIER, [&store](error_indicator e) {
    ASSERT_FALSE(e);

    vector<std::string> configs = store.get_all();
    ASSERT_EQ(0, configs.size());
  });

  io_service_.run();
}

/*
 * ### Get single config ###
 */
TEST_F(DbConfigStoreTest, GetSingleConfig) {
  db_config_store store(DB_FILE, &io_service_);

  store.get(IDENTIFIER, [&store](string config, error_indicator e) {
    ASSERT_FALSE(e);

    EXPECT_EQ(EXPECTED_CONFIG, config);
  });

  io_service_.run();
}

/*
 * ### Update attribute ###
 */
TEST_F(DbConfigStoreTest, UpdateAttribute) {
  db_config_store store(DB_FILE, &io_service_);

  store.update_attribute(testbot_, "crime", "crime", "testcrime", [&store](error_indicator e) {
    ASSERT_FALSE(e);

    store.get(IDENTIFIER, [&store](string config, error_indicator e) {
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

  store.add(testbot_, [&store](error_indicator e) {
    ASSERT_FALSE(e);

    vector<std::string> configs = store.get_all();
    ASSERT_EQ(1, configs.size());
    EXPECT_EQ(EXPECTED_CONFIG, configs.at(0));
  });

  io_service_.run();
}

/*
 * ### Remove inexistent ###
 */
TEST_F(DbConfigStoreTest, RemoveNonexisting) {
  db_config_store store(DB_FILE, &io_service_);

  store.remove("not_in_db", [&store](error_indicator e) {
    ASSERT_FALSE(e);

    vector<std::string> configs = store.get_all();
    ASSERT_EQ(1, configs.size());
  });

  io_service_.run();
}
