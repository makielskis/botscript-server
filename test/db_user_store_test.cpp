#include "gtest/gtest.h"

#include <ctime>

#include "boost/asio/io_service.hpp"
#include "boost/filesystem.hpp"

#include "../src/storage/db_user_store.h"
#include "../src/storage/kc_wrapper.h"


#define USERNAME ("testuser")
#define PASSWORD ("password")
#define PW_HASH  ("5f4dcc3b5aa765d61d8327deb882cf99")
#define EMAIL    ("a@b.cd")

using namespace std;
using namespace botscript_server;

class UserStoreTest : public testing::Test {
 protected:
  virtual void SetUp() override {
    addTestUser();
  }

  virtual void TearDown() override {
    boost::filesystem::remove("test.kch");
  }

  void addTestUser() {
    string actual_sid;

    {
      db_user_store store("test.kch", &io_service_);

      store.add_user(USERNAME, PASSWORD, EMAIL, [&actual_sid, &store](string sid, error_indicator e) {
        actual_sid = sid;
        ASSERT_FALSE(e);
        EXPECT_EQ(store.user_sid().at(USERNAME), sid);
        EXPECT_EQ(store.sid_user().at(actual_sid), USERNAME);
        EXPECT_GT(store.sid_expire().at(actual_sid), time(nullptr));
      });

      io_service_.run();
      io_service_.reset();
    }

    db db("test.kch");
    entry users = db.get_entry("users");

    EXPECT_EQ(PW_HASH, users[USERNAME]["password"].val());
    EXPECT_EQ("a@b.cd", users[USERNAME]["email"].val());
    EXPECT_EQ(32, actual_sid.length());
  }

  boost::asio::io_service io_service_;
};

TEST_F(UserStoreTest, AddUserTwiceTest) {
  {
    db_user_store store("test.kch", &io_service_);
    store.add_user(USERNAME, "other", EMAIL, [](string sid, error_indicator e) {
      ASSERT_TRUE(e);
    });

    io_service_.run();
    io_service_.reset();
  }

  db db("test.kch");
  entry users = db.get_entry("users");
  EXPECT_EQ(PW_HASH, users[USERNAME]["password"].val());
}

TEST_F(UserStoreTest, SuccessfulLoginTest) {
  db_user_store store("test.kch", &io_service_);
  store.login(USERNAME, PASSWORD, [&store](string sid, error_indicator error) {
    const string& store_sid = store.user_sid().at(USERNAME);
    const time_t& store_expire = store.sid_expire().at(sid);
    const string& store_username = store.sid_user().at(sid);

    EXPECT_EQ(store_sid, sid);
    EXPECT_EQ(store_username, USERNAME);
    EXPECT_GT(store_expire, time(nullptr));
    EXPECT_EQ(32, sid.length());
    EXPECT_FALSE(error);
  });

  io_service_.run();
  io_service_.reset();
}

TEST_F(UserStoreTest, WrongPasswordLoginTest) {
  db_user_store store("test.kch", &io_service_);
  store.login(USERNAME, "wrong", [&store](string sid, error_indicator error) {
    EXPECT_TRUE(store.user_sid().empty());
    EXPECT_TRUE(store.sid_user().empty());
    EXPECT_TRUE(store.sid_expire().empty());
    ASSERT_TRUE(error);
  });

  io_service_.run();
  io_service_.reset();
}

TEST_F(UserStoreTest, UserNotExistentLoginTest) {
  db_user_store store("test.kch", &io_service_);
  store.login("abc", PASSWORD, [&store](string sid, error_indicator error) {
    EXPECT_TRUE(store.user_sid().empty());
    EXPECT_TRUE(store.sid_user().empty());
    EXPECT_TRUE(store.sid_expire().empty());
    ASSERT_TRUE(error);
  });

  io_service_.run();
  io_service_.reset();
}

TEST_F(UserStoreTest, DoubleLoginNotPossibleTest) {
  db_user_store store("test.kch", &io_service_);
  store.login(USERNAME, PASSWORD, [&store](string sid1, error_indicator error) {
    ASSERT_FALSE(error);
    store.login(USERNAME, PASSWORD, [&store, sid1](string sid2, error_indicator error) {
      ASSERT_FALSE(error);
      EXPECT_EQ(1, store.user_sid().size());
      EXPECT_EQ(1, store.sid_user().size());
      EXPECT_EQ(1, store.sid_expire().size());
      EXPECT_FALSE(sid1 == sid2);

      store.logout(sid1, [&store](error_indicator error) {
        EXPECT_TRUE(error);
      });

      store.logout(sid2, [&store](error_indicator error) {
        EXPECT_FALSE(error);
      });
    });
  });

  io_service_.run();
  io_service_.reset();
}

TEST_F(UserStoreTest, SuccessfulLogoutTest) {
  db_user_store store("test.kch", &io_service_);
  store.login(USERNAME, PASSWORD, [&store](string sid, error_indicator error) {
    store.logout(sid, [&store, &sid](error_indicator error) {
      const auto store_sid = store.user_sid().find(USERNAME);
      const auto store_expire = store.sid_expire().find(sid);
      const auto store_username = store.sid_user().find(sid);

      EXPECT_TRUE(store.user_sid().empty());
      EXPECT_TRUE(store.sid_user().empty());
      EXPECT_TRUE(store.sid_expire().empty());
      EXPECT_EQ(store.user_sid().cend(), store_sid);
      EXPECT_EQ(store.sid_expire().cend(), store_expire);
      EXPECT_EQ(store.sid_user().cend(), store_username);
      EXPECT_FALSE(error);
    });
  });

  io_service_.run();
  io_service_.reset();
}

TEST_F(UserStoreTest, WrongSessionLogoutTest) {
  db_user_store store("test.kch", &io_service_);
  store.login(USERNAME, PASSWORD, [&store](string sid, error_indicator error) {
    store.logout("wrongsession", [&store, &sid](error_indicator error) {
      EXPECT_EQ(1, store.user_sid().size());
      EXPECT_EQ(1, store.sid_user().size());
      EXPECT_EQ(1, store.sid_expire().size());
      ASSERT_TRUE(error);
    });
  });

  io_service_.run();
  io_service_.reset();
}

TEST_F(UserStoreTest, SuccessfulRemoveUserTest) {
  {
    db_user_store store("test.kch", &io_service_);
    store.login(USERNAME, PASSWORD, [&store](string sid, error_indicator error) {
      ASSERT_FALSE(error);
      store.remove_user(sid, PASSWORD, [&store](error_indicator error) {
        EXPECT_FALSE(error);
        EXPECT_TRUE(store.user_sid().empty());
        EXPECT_TRUE(store.sid_user().empty());
        EXPECT_TRUE(store.sid_expire().empty());
      });
    });

    io_service_.run();
    io_service_.reset();
  }

  db db("test.kch");
  entry users = db.get_entry("users");
  EXPECT_EQ(0, db.count());
  EXPECT_FALSE(users[USERNAME]["password"].exists());
  EXPECT_FALSE(users[USERNAME]["email"].exists());
  EXPECT_FALSE(users[USERNAME]["bots"].exists());
}
