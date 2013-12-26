#include <memory>

#include "gtest/gtest.h"

#include "dust/storage/mem_store.h"
#include "dust/document.h"

#include "../src/user.h"

#define PW ("password")
#define PW_HASH ("5f4dcc3b5aa765d61d8327deb882cf99")
#define NEW_PW ("other")
#define NEW_PW_HASH ("795f3202b17cb6bc3d4b771d8c6c9eaf")
#define EMAIL ("mail@foo.bar")
#define NEW_EMAIL ("mail@bar.foo")

using namespace dust;
using namespace botscript_server;

class user_test : public testing::Test {
 public:
  user_test()
      : store_(std::make_shared<mem_store>()),
        users_(store_, "users") {
  }

 protected:
  std::shared_ptr<key_value_store> store_;
  document users_;
};

TEST_F(user_test, construct_from_existing_test) {
  users_["test_user"]["password"] = PW_HASH;
  users_["test_user"]["email"] = EMAIL;

  user u(users_["test_user"]);
  EXPECT_EQ("test_user", u.username());
  EXPECT_EQ(EMAIL, u.email());
}

TEST_F(user_test, constructor_from_existing_fail_test) {
  EXPECT_THROW({
    user u(users_["test_user"]);
  }, boost::system::system_error);
}

TEST_F(user_test, construct_new_test) {
  user u(users_["test_user"], PW, EMAIL);

  EXPECT_EQ("test_user", u.username());
  EXPECT_EQ(PW_HASH, users_["test_user"]["password"].val());
  EXPECT_EQ(EMAIL, users_["test_user"]["email"].val());
}

TEST_F(user_test, constructor_new_at_existing_fail_test) {
  users_["test_user"]["email"] = EMAIL;
  users_["test_user"]["password"] = PW_HASH;
  users_["test_user"]["session"]["id"] = "some_id";
  users_["test_user"]["session"]["expire"] = "0";

  EXPECT_THROW({
    user u(users_["test_user"], PW, EMAIL);
  }, boost::system::system_error);
}

TEST_F(user_test, check_password_test) {
  user u(users_["test_user"], PW, EMAIL);

  ASSERT_TRUE(u.check_password(PW));
  ASSERT_FALSE(u.check_password("wrong"));
}

TEST_F(user_test, change_password_test_success) {
  user u(users_["test_user"], PW, EMAIL);

  u.password(PW, NEW_PW);

  EXPECT_EQ(NEW_PW_HASH, users_["test_user"]["password"].val());
}

TEST_F(user_test, change_password_test_failure) {
  user u(users_["test_user"], PW, EMAIL);

  EXPECT_THROW({
    u.password("wrong", ".");
  }, boost::system::system_error);
}


TEST_F(user_test, change_email_test_success) {
  user u(users_["test_user"], PW, EMAIL);

  u.email(PW, NEW_EMAIL);

  EXPECT_EQ(NEW_EMAIL, users_["test_user"]["email"].val());
}

TEST_F(user_test, change_email_test_failure) {
  user u(users_["test_user"], PW, EMAIL);

  EXPECT_THROW({
    u.email("wrong", ".");
  }, boost::system::system_error);
}

TEST_F(user_test, new_user_session_test) {
  user u(users_["test_user"], PW, EMAIL);

  std::time_t expire = time(nullptr) + EXPIRE;
  EXPECT_GE(expire, u.session_expire());
  EXPECT_TRUE(u.session_active());
}

TEST_F(user_test, renew_user_session_test) {
  users_["test_user"]["email"] = EMAIL;
  users_["test_user"]["password"] = PW_HASH;
  users_["test_user"]["session"]["id"] = "some_id";
  users_["test_user"]["session"]["expire"] = "0";

  user u(users_["test_user"]);

  EXPECT_FALSE(u.session_active());

  u.renew_session();

  EXPECT_TRUE(u.session_active());
}
