#include "gtest/gtest.h"

#include <ctime>

#include "../src/storage/db_user_store.h"
#include "../src/storage/kc_wrapper.h"

#include "boost/asio/io_service.hpp"

using namespace std;
using namespace botscript_server;

class UserStoreTest : public testing::Test {
 protected:
  virtual void SetUp() {
    addTestUser();
  }

  void addTestUser() {
    string actual_sid;
    bool error = false;
    string store_sid;
    time_t store_expire;
    string store_username;
    {
      db_user_store store("test.kch", &io_service_);

      store.add_user("testuser", "password", "a@b.cd", [&actual_sid, &error](string sid, error_indicator e) {
        actual_sid = sid;
        error = e;
      });

      io_service_.run();

      store_sid = store.user_sid().at("testuser");
      store_expire = store.sid_expire().at(actual_sid);
      store_username = store.sid_user().at(actual_sid);
    }

    db db("test.kch");
    entry users = db.get_entry("users");

    EXPECT_EQ(store_sid, actual_sid);
    EXPECT_EQ(store_username, "testuser");
    EXPECT_GT(store_expire, time(nullptr));
    EXPECT_EQ("5f4dcc3b5aa765d61d8327deb882cf99", users["testuser"]["password"].val());
    EXPECT_EQ("a@b.cd", users["testuser"]["email"].val());
    EXPECT_EQ(32, actual_sid.length());
    EXPECT_FALSE(error);
  }

  boost::asio::io_service io_service_;
};

TEST_F(UserStoreTest, LoginTest) {
  db_user_store store("test.kch", &io_service_);
  store.login("testuser", "password", [&store](string sid, error_indicator error) {
    const string& store_sid = store.user_sid().at("testuser");
    const time_t& store_expire = store.sid_expire().at(sid);
    const string& store_username = store.sid_user().at(sid);

    EXPECT_EQ(store_sid, sid);
    EXPECT_EQ(store_username, "testuser");
    EXPECT_GT(store_expire, time(nullptr));
    EXPECT_EQ(32, sid.length());
    EXPECT_FALSE(error);
  });

  io_service_.run();
}

TEST_F(UserStoreTest, LogoutTest) {
  db_user_store store("test.kch", &io_service_);
  store.login("testuser", "password", [&store](string sid, error_indicator error) {
    store.logout(sid, [&store, &sid](error_indicator error) {
      const auto store_sid = store.user_sid().find("testuser");
      const auto store_expire = store.sid_expire().find(sid);
      const auto store_username = store.sid_user().find(sid);

      EXPECT_EQ(store.user_sid().cend(), store_sid);
      EXPECT_EQ(store.sid_expire().cend(), store_expire);
      EXPECT_EQ(store.sid_user().cend(), store_username);
      EXPECT_FALSE(error);
    });
  });

  io_service_.run();
}

