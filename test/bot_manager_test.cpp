#include "gtest/gtest.h"

#include "boost/asio/io_service.hpp"

#include "../src/error.h"
#include "../src/storage/db_config_store.h"
#include "../src/storage/db_user_store.h"
#include "../src/bot_manager.h"
#include "../src/messages/outgoing_msgs.h"

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

    io_service_.run();
    io_service_.reset();
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
  login_msg m(USER, PW);
  bot_manager_.handle_login_msg(m, [](std::vector<outgoing_msg_ptr> response) {
    ASSERT_EQ(1u, response.size());
    session_msg* sid = dynamic_cast<session_msg*>(response[0].get());
    ASSERT_NE(nullptr, sid);
    ASSERT_EQ(32u, sid->sid().length());
  });

  io_service_.run();
  io_service_.reset();
}

/*
 * ### Login failed ###
 */
TEST_F(BotManagerTest, FailedLoginTest) {
  login_msg m("not_registered", "not_relevant");
  bot_manager_.handle_login_msg(m, [](std::vector<outgoing_msg_ptr> response) {
    ASSERT_EQ(1u, response.size());
    failure_msg* fail = dynamic_cast<failure_msg*>(response[0].get());
    ASSERT_NE(nullptr, fail);
    ASSERT_EQ(error::error_code_t::user_not_found, fail->error_code());
  });

  io_service_.run();
  io_service_.reset();
}
