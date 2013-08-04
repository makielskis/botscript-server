#include "gtest/gtest.h"

#include "boost/asio/io_service.hpp"

#include "../src/storage/db_config_store.h"
#include "../src/storage/db_user_store.h"
#include "../src/bot_manager.h"

#define DB_PATH ("*")

#define USER  ("user")
#define PW    ("pw")
#define MAIL  ("mail")

using namespace std;
using namespace botscript_server;
using boost::system::error_code;

class BotManagerTest : public testing::Test {
 public:
  BotManagerTest()
      : config_store_(DB_PATH, &io_service_),
        user_store_(DB_PATH, &io_service_),
        bot_manager_(config_store_, user_store_, &io_service_) {
  }

 protected:
  virtual void SetUp() override {
    addUserTest();
  }

  virtual void TearDown() override {
  }

  void addUserTest() {
    user_store_.add_user(USER, PW, MAIL, [=](std::string s, error_code e) {
      ASSERT_FALSE(e);
    });

    io_service_.run_one();
  }

  boost::asio::io_service io_service_;
  db_config_store config_store_;
  db_user_store user_store_;
  bot_manager bot_manager_;
};

/*
 * ### Login successful ###
 */
TEST_F(BotManagerTest, SuccessfulLoginTest) {
  ASSERT_TRUE(true);
}
