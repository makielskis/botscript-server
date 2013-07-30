#include "gtest/gtest.h"

#include "../src/error.h"

using namespace botscript_server;
using boost::system::error_code;

TEST(error_test, equals_test) {
  error_code ec = error::password_wrong;
  EXPECT_EQ(error::password_wrong, ec);
  EXPECT_EQ("Password wrong", ec.message());
}

TEST(error_test, category_test) {
  error_code ec1 = error::session_id_not_available;
  error_code ec2 = error::session_id_timed_out;

  EXPECT_EQ(error::session_error, ec1);
  EXPECT_EQ(error::session_error, ec2);
  EXPECT_FALSE(error::password_wrong == ec2);
}
