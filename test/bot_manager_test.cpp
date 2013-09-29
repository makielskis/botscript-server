#include <vector>
#include <string>
#include <functional>
#include <memory>

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

#define CONFIG "{\"username\":\"login_username\",\"password\":\"login_password\",\"package\":\"ts\",\"server\":\"http://example.com\",\"inactive\":false,\"modules\":{\"base\":{\"wait_time_factor\":\"1.00\",\"proxy\":\"8.8.8.8:53\"}}}"

using namespace std;
using namespace botscript_server;
using boost::system::error_code;

class BotManagerTest : public testing::Test {
 public:
  BotManagerTest()
      : config_store_(DB_PATH, &io_service_),
        user_store_(DB_PATH, &io_service_),
        bot_manager_("../test/packages",
                     config_store_,
                     user_store_,
                     std::bind(&BotManagerTest::activity_callback,
                               this,
                               std::placeholders::_1,
                               std::placeholders::_2),
                     std::bind(&BotManagerTest::session_close_callback,
                               this,
                               std::placeholders::_1),
                     &io_service_) {
  }

 protected:
  virtual void SetUp() override {
    successfulRegistrationTest();
    successfulLoginTest();
    successfulBotCreationTest();
  }

  virtual void TearDown() override {
    successfulBotDeletionTest();
    boost::filesystem::remove(DB_PATH);
  }

  void activity_callback(std::string sid, std::vector<outgoing_msg_ptr> msgs) {
    std::cout << "outgoing messages for " << sid << "\n";
    for (const auto& msg : msgs) {
      std::cout << msg->to_json() << "\n";
    }
    std::cout << "\n";
  }

  void session_close_callback(const std::string& sid) {
    std::cout << "session " << sid << " closed\n";
  }

  void successfulRegistrationTest() {
    register_msg m(USER, PW, MAIL);
    bot_manager_.handle_register_msg(m, [](std::string sid, std::vector<outgoing_msg_ptr> response) {
      ASSERT_FALSE(sid.empty());
      ASSERT_EQ(4u, response.size());
      ASSERT_NE(nullptr, dynamic_cast<session_msg*>(response[0].get()));
    });

    io_service_.run();
    io_service_.reset();
  }

  void successfulLoginTest() {
    login_msg m(USER, PW);
    bot_manager_.handle_login_msg(m, [=](std::string sid, std::vector<outgoing_msg_ptr> response) {
      ASSERT_EQ(4u, response.size());

      // Check session ID.
      ASSERT_FALSE(sid.empty());
      sid_ = sid;
      session_msg* sid_msg = dynamic_cast<session_msg*>(response[0].get());
      ASSERT_NE(nullptr, sid_msg);
      ASSERT_EQ(32u, sid_msg->sid().length());

      // Check account message containing the email.
      account_msg* account = dynamic_cast<account_msg*>(response[1].get());
      ASSERT_EQ(MAIL, account->email());

      // Check packages.
      packages_msg* packages = dynamic_cast<packages_msg*>(response[2].get());
      ASSERT_NE(nullptr, packages);
      ASSERT_EQ(2u, packages->packages().size());

      {
        rapidjson::Document d;
        ASSERT_FALSE(d.Parse<0>(packages->packages().at(0).c_str()).HasParseError());

        ASSERT_TRUE(d.IsObject());
        ASSERT_TRUE(d.HasMember("name"));
        ASSERT_TRUE(d["name"].IsString());
        ASSERT_TRUE(d.HasMember("servers"));
        ASSERT_TRUE(d["servers"].IsArray());
        ASSERT_TRUE(d.HasMember("base"));
        ASSERT_TRUE(d["base"].IsObject());
      }
      {
        rapidjson::Document d;
        ASSERT_FALSE(d.Parse<0>(packages->packages().at(1).c_str()).HasParseError());

        ASSERT_TRUE(d.IsObject());
        ASSERT_TRUE(d.HasMember("name"));
        ASSERT_TRUE(d["name"].IsString());
        ASSERT_TRUE(d.HasMember("servers"));
        ASSERT_TRUE(d["servers"].IsArray());
        ASSERT_TRUE(d.HasMember("base"));
        ASSERT_TRUE(d["base"].IsObject());
      }

      // Check bots.
      bots_msg* bots = dynamic_cast<bots_msg*>(response[3].get());
      ASSERT_NE(nullptr, bots);
      ASSERT_EQ(0u, bots->bot_configs().size());
    });

    io_service_.run();
    io_service_.reset();
  }

  void successfulBotCreationTest() {
    create_bot_msg m(sid_, CONFIG);
    bot_manager_.handle_create_bot_msg(m, [=](std::vector<outgoing_msg_ptr> response) {
      ASSERT_EQ(1u, response.size());

      // Check bots.
      bots_msg* bots = dynamic_cast<bots_msg*>(response[0].get());
      ASSERT_NE(nullptr, bots);
      ASSERT_EQ(1u, bots->bot_configs().size());
    });

    io_service_.run();
    io_service_.reset();
  }

  void successfulBotDeletionTest() {
    delete_bot_msg m(sid_, "ts_ex_login_username");
    bot_manager_.handle_delete_bot_msg(m, [=](std::vector<outgoing_msg_ptr> response) {
      ASSERT_EQ(1u, response.size());

      // Check bots.
      bots_msg* bots = dynamic_cast<bots_msg*>(response[0].get());
      ASSERT_NE(nullptr, bots);
      ASSERT_EQ(0u, bots->bot_configs().size());
    });

    io_service_.run();
    io_service_.reset();
  }

  boost::asio::io_service io_service_;
  db_config_store config_store_;
  db_user_store user_store_;
  bot_manager bot_manager_;
  std::string sid_;
};

/*
 * ### Login failed ###
 */
TEST_F(BotManagerTest, FailedLoginTest) {
  login_msg m("not_registered", "not_relevant");
  bot_manager_.handle_login_msg(m, [](std::string sid, std::vector<outgoing_msg_ptr> response) {
    ASSERT_EQ(1u, response.size());
    failure_msg* fail = dynamic_cast<failure_msg*>(response[0].get());
    ASSERT_NE(nullptr, fail);
    ASSERT_EQ(error::error_code_t::user_not_found, fail->error_code());
  });

  io_service_.run();
  io_service_.reset();
}

/*
 * ### Registration failed ###
 */
TEST_F(BotManagerTest, FailedRegistration) {
  register_msg m(USER, PW, MAIL);
  bot_manager_.handle_register_msg(m, [](std::string sid, std::vector<outgoing_msg_ptr> response) {
    ASSERT_EQ(1u, response.size());
    failure_msg* fail = dynamic_cast<failure_msg*>(response[0].get());
    ASSERT_NE(nullptr, fail);
    ASSERT_EQ(error::error_code_t::username_already_taken, fail->error_code());
  });

  io_service_.run();
  io_service_.reset();
}
