#include "gtest/gtest.h"

#include <ctime>
#include <string>
#include <vector>
#include <algorithm>

#include "boost/asio/io_service.hpp"
#include "boost/filesystem.hpp"

#include "../src/storage/db_user_store.h"
#include "../src/storage/kc_wrapper.h"


#define USERNAME       ("testuser")
#define PW             ("password")
#define PW_HASH        ("5f4dcc3b5aa765d61d8327deb882cf99")
#define OTHER_PW       ("other")
#define OTHER_PW_HASH  ("795f3202b17cb6bc3d4b771d8c6c9eaf")
#define EMAIL          ("a@b.cd")
#define OTHER_EMAIL    ("e@f.gh")
#define BOT1           ("pg_hh_test")
#define BOT2           ("kv_sf_test")
#define BOT3           ("pg_ml_test")
#define BOT4           ("pg_ny_test")

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

      store.add_user(USERNAME, PW, EMAIL, [&actual_sid, &store](string sid, error_indicator e) {
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

/*
 * ### Add user ###
 */
TEST_F(UserStoreTest, AddUserTwiceTest) {
  {
    db_user_store store("test.kch", &io_service_);
    store.add_user(USERNAME, OTHER_PW, EMAIL, [](string sid, error_indicator e) {
      ASSERT_TRUE(e);
    });

    io_service_.run();
    io_service_.reset();
  }

  db db("test.kch");
  entry users = db.get_entry("users");
  EXPECT_EQ(PW_HASH, users[USERNAME]["password"].val());
}


/*
 * ### Login ###
 */
TEST_F(UserStoreTest, SuccessfulLoginTest) {
  db_user_store store("test.kch", &io_service_);
  store.login(USERNAME, PW, [&store](string sid, error_indicator error) {
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
  store.login("abc", PW, [&store](string sid, error_indicator error) {
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
  store.login(USERNAME, PW, [&store](string sid1, error_indicator error) {
    ASSERT_FALSE(error);
    store.login(USERNAME, PW, [&store, sid1](string sid2, error_indicator error) {
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


/*
 * ### Logout user ###
 */
TEST_F(UserStoreTest, SuccessfulLogoutTest) {
  db_user_store store("test.kch", &io_service_);
  store.login(USERNAME, PW, [&store](string sid, error_indicator error) {
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
  store.login(USERNAME, PW, [&store](string sid, error_indicator error) {
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


/*
 * ### Remove user ###
 */
TEST_F(UserStoreTest, SuccessfulRemoveUserTest) {
  {
    db_user_store store("test.kch", &io_service_);
    store.login(USERNAME, PW, [&store](string sid, error_indicator error) {
      ASSERT_FALSE(error);
      store.remove_user(sid, PW, [&store](error_indicator error) {
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

TEST_F(UserStoreTest, WrongPasswordRemoveUserTest) {
  {
    db_user_store store("test.kch", &io_service_);
    store.login(USERNAME, PW, [&store](string sid, error_indicator error) {
      ASSERT_FALSE(error);
      store.remove_user(sid, "wrong", [&store](error_indicator error) {
        EXPECT_TRUE(error);
        EXPECT_EQ(1, store.user_sid().size());
        EXPECT_EQ(1, store.sid_user().size());
        EXPECT_EQ(1, store.sid_expire().size());
      });
    });

    io_service_.run();
    io_service_.reset();
  }

  db db("test.kch");
  entry users = db.get_entry("users");
  EXPECT_TRUE(users[USERNAME]["password"].exists());
  EXPECT_TRUE(users[USERNAME]["email"].exists());
}

TEST_F(UserStoreTest, WrongSessionRemoveUserTest) {
  {
    db_user_store store("test.kch", &io_service_);
    store.login(USERNAME, PW, [&store](string sid, error_indicator error) {
      ASSERT_FALSE(error);
      store.remove_user("wrongsession", PW, [&store](error_indicator error) {
        EXPECT_TRUE(error);
        EXPECT_EQ(1, store.user_sid().size());
        EXPECT_EQ(1, store.sid_user().size());
        EXPECT_EQ(1, store.sid_expire().size());
      });
    });

    io_service_.run();
    io_service_.reset();
  }

  db db("test.kch");
  entry users = db.get_entry("users");
  EXPECT_TRUE(users[USERNAME]["password"].exists());
  EXPECT_TRUE(users[USERNAME]["email"].exists());
}


/*
 * ### Set password ###
 */
TEST_F(UserStoreTest, SuccessfulSetPasswordTest) {
  {
    db_user_store store("test.kch", &io_service_);
    store.login(USERNAME, PW, [&store](string sid, error_indicator error) {
      ASSERT_FALSE(error);
      store.set_password(sid, PW, OTHER_PW, [sid, &store](error_indicator error) {
        ASSERT_FALSE(error);

        store.logout(sid, [&store](error_indicator error) {
          ASSERT_FALSE(error);

          store.login(USERNAME, PW, [](string sid, error_indicator error) {
            EXPECT_TRUE(error);
          });

          store.login(USERNAME, OTHER_PW, [](string sid, error_indicator error) {
            EXPECT_FALSE(error);
          });
        });
      });
    });

    io_service_.run();
    io_service_.reset();
  }

  db db("test.kch");
  entry users = db.get_entry("users");
  EXPECT_EQ(users[USERNAME]["password"].val(), OTHER_PW_HASH);
}

TEST_F(UserStoreTest, WrongOldPasswordSetPasswordTest) {
  {
    db_user_store store("test.kch", &io_service_);
    store.login(USERNAME, PW, [&store](string sid, error_indicator error) {
      ASSERT_FALSE(error);
      store.set_password(sid, "wrong", OTHER_PW, [sid, &store](error_indicator error) {
        EXPECT_TRUE(error);
      });
    });

    io_service_.run();
    io_service_.reset();
  }

  db db("test.kch");
  entry users = db.get_entry("users");
  EXPECT_EQ(users[USERNAME]["password"].val(), PW_HASH);
}

/*
 * ### set/get email ###
 */
TEST_F(UserStoreTest, ChangeEmailTest) {
  {
    db_user_store store("test.kch", &io_service_);
    store.login(USERNAME, PW, [&store](string sid, error_indicator error) {
      ASSERT_FALSE(error);

      store.get_email(sid, [sid, &store](string email, error_indicator error) {
        ASSERT_FALSE(error);

        ASSERT_EQ(EMAIL, email);

        store.set_email(sid, PW, OTHER_EMAIL, [sid, &store](error_indicator error) {
          ASSERT_FALSE(error);
        });
      });
    });

    io_service_.run();
    io_service_.reset();
  }

  db db("test.kch");
  entry users = db.get_entry("users");
  EXPECT_EQ(users[USERNAME]["email"].val(), OTHER_EMAIL);
}

/*
 * ### add/remove/get bot ###
 */
TEST_F(UserStoreTest, BotManagementTest) {
  {
    db_user_store store("test.kch", &io_service_);
    store.login(USERNAME, PW, [&store](string sid, error_indicator error) {
      // Add bot1 -> BOT1
      store.add_bot(sid, BOT1, [sid, &store](error_indicator error) {
        ASSERT_FALSE(error);

        // Add bot2 -> BOT1,BOT2
        store.add_bot(sid, BOT2, [sid, &store](error_indicator error) {
          ASSERT_FALSE(error);

          // Add bot3 -> BOT1,BOT2,BOT3
          store.add_bot(sid, BOT3, [sid, &store](error_indicator error) {
            ASSERT_FALSE(error);

            // Add bot4 -> BOT1,BOT2,BOT3,BOT4
            store.add_bot(sid, BOT4, [sid, &store](error_indicator error) {
              ASSERT_FALSE(error);

              // Check if all bots are there
              store.get_bots(sid, [sid, &store](vector<string> bots, error_indicator error) {
                ASSERT_FALSE(error);

                EXPECT_EQ(4, bots.size());
                EXPECT_FALSE(std::find(bots.begin(), bots.end(), BOT1) == bots.end());
                EXPECT_FALSE(std::find(bots.begin(), bots.end(), BOT2) == bots.end());
                EXPECT_FALSE(std::find(bots.begin(), bots.end(), BOT3) == bots.end());
                EXPECT_FALSE(std::find(bots.begin(), bots.end(), BOT4) == bots.end());

                // Remove bot4 -> BOT1,BOT2,BOT3
                store.remove_bot(sid, BOT4, [sid, &store](error_indicator error) {
                  ASSERT_FALSE(error);

                  // Check if all bots except bot2 are there
                  store.get_bots(sid, [sid, &store](vector<string> bots, error_indicator error) {
                    ASSERT_FALSE(error);

                    EXPECT_EQ(3, bots.size());
                    EXPECT_FALSE(std::find(bots.begin(), bots.end(), BOT1) == bots.end());
                    EXPECT_FALSE(std::find(bots.begin(), bots.end(), BOT2) == bots.end());
                    EXPECT_FALSE(std::find(bots.begin(), bots.end(), BOT3) == bots.end());
                    EXPECT_TRUE(std::find(bots.begin(), bots.end(), BOT4) == bots.end());

                    // Remove bot2 -> BOT1,BOT3
                    store.remove_bot(sid, BOT2, [sid, &store](error_indicator error) {
                      ASSERT_FALSE(error);

                      // Check if all bots except bot2 are there
                      store.get_bots(sid, [sid, &store](vector<string> bots, error_indicator error) {
                        ASSERT_FALSE(error);

                        EXPECT_EQ(2, bots.size());
                        EXPECT_FALSE(std::find(bots.begin(), bots.end(), BOT1) == bots.end());
                        EXPECT_TRUE(std::find(bots.begin(), bots.end(), BOT2) == bots.end());
                        EXPECT_FALSE(std::find(bots.begin(), bots.end(), BOT3) == bots.end());
                        EXPECT_TRUE(std::find(bots.begin(), bots.end(), BOT4) == bots.end());

                        // Remove bot1 -> BOT3
                        store.remove_bot(sid, BOT1, [sid, &store](error_indicator error) {
                          ASSERT_FALSE(error);

                          // Check if all bots except bot2 and bot1 are there
                          store.get_bots(sid, [sid, &store](vector<string> bots, error_indicator error) {
                            ASSERT_FALSE(error);

                            EXPECT_EQ(1, bots.size());
                            EXPECT_TRUE(std::find(bots.begin(), bots.end(), BOT1) == bots.end());
                            EXPECT_TRUE(std::find(bots.begin(), bots.end(), BOT2) == bots.end());
                            EXPECT_FALSE(std::find(bots.begin(), bots.end(), BOT3) == bots.end());
                            EXPECT_TRUE(std::find(bots.begin(), bots.end(), BOT4) == bots.end());

                            // Remove bot3 -> no bots
                            store.remove_bot(sid, BOT3, [sid, &store](error_indicator error) {
                              ASSERT_FALSE(error);

                              // Check that all bots are gone
                              store.get_bots(sid, [sid, &store](vector<string> bots, error_indicator error) {
                                ASSERT_FALSE(error);

                                EXPECT_EQ(0, bots.size());
                              });
                            });
                          });
                        });
                      });
                    });
                  });
                });
              });
            });
          });
        });
      });
    });

    io_service_.run();
    io_service_.reset();
  }
}
