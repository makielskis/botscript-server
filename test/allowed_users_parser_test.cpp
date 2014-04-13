#include <sstream>

#include "boost/system/system_error.hpp"

#include "gtest/gtest.h"

#include "../src/allowed_users_parser.h"
#include "../src/error.h"

using namespace botscript_server;

char const* allowed_users = R"({
  "allowed_users": [
    {
      "name": "foo",
      "allowed_bots": [],
      "all_bots_allowed": true
    },
    {
      "name": "bar",
      "allowed_bots": [
        "pg_hh_a",
        "pg_sy_b"
      ],
      "all_bots_allowed": false
    },
    {
      "name": "test",
      "allowed_bots": [],
      "all_bots_allowed": false
    }
  ]
})";

TEST(allowed_users_test, parse_error_check) {
  EXPECT_THROW({
    std::stringstream ss;
    ss << "{ }";
    check_user_allowed(ss, "oclife");
  }, boost::system::system_error);
}

TEST(allowed_users_test, allowed_users) {
  EXPECT_NO_THROW({
    std::stringstream ss;
    ss << allowed_users;
    check_user_allowed(ss, "foo");
  });

  EXPECT_NO_THROW({
    std::stringstream ss;
    ss << allowed_users;
    check_user_allowed(ss, "bar");
  });

  EXPECT_NO_THROW({
    std::stringstream ss;
    ss << allowed_users;
    check_user_allowed(ss, "test");
  });
}

TEST(allowed_users_test, all_allowed) {
  EXPECT_NO_THROW({
    std::stringstream ss;
    ss << allowed_users;
    check_bot_allowed(ss, "foo", "dontcare");
  });
}

TEST(allowed_users_test, user_with_allowed_bot) {
  EXPECT_THROW({
    try {
      std::stringstream ss;
      ss << allowed_users;
      check_bot_allowed(ss, "bar", "123");
    } catch (boost::system::system_error const& e) {
      ASSERT_EQ(e.code(), error::bot_not_allowed);
      throw;
    }
  }, boost::system::system_error);

  EXPECT_NO_THROW({
    std::stringstream ss;
    ss << allowed_users;
    check_bot_allowed(ss, "bar", "pg_hh_a");
  });

  EXPECT_NO_THROW({
    std::stringstream ss;
    ss << allowed_users;
    check_bot_allowed(ss, "bar", "pg_sy_b");
  });
}

TEST(allowed_users_test, user_with_not_allowed_bot) {
  EXPECT_THROW({
    try {
      std::stringstream ss;
      ss << allowed_users;
      check_bot_allowed(ss, "test", "dontcare");
    } catch (boost::system::system_error const& e) {
      ASSERT_EQ(e.code(), error::bot_not_allowed);
      throw;
    }
  }, boost::system::system_error);
}


TEST(allowed_users_test, bot_with_user_not_allowed) {
  EXPECT_THROW({
    try {
      std::stringstream ss;
      ss << allowed_users;
      check_bot_allowed(ss, "dontcare", "dontcare");
    } catch (boost::system::system_error const& e) {
      ASSERT_EQ(e.code(), error::user_not_allowed);
      throw;
    }
  }, boost::system::system_error);
}
