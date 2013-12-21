#include <memory>

#include "gtest/gtest.h"

#include "dust/storage/mem_store.h"
#include "dust/document.h"

#include "../src/user.h"

#define PW ("password")
#define PW_HASH ("5f4dcc3b5aa765d61d8327deb882cf99")

using namespace dust;
using namespace botscript_server;

class user_test: public testing::Test {
public:
  user_test()
      : store_(std::make_shared<mem_store>()),
        users_(store_, "users") {
    std::cout << "Setup" << std::endl;
  }

  ~user_test() {
    std::cout << "Shutdown" << std::endl;
  }

protected:
  std::shared_ptr<key_value_store> store_;
  document users_;
};

TEST_F(user_test, construct_from_existing_test) {
  users_["test_user"]["password"] = PW_HASH;

  std::cout << "1" << std::endl;
  user u(users_["test_user"]);
  std::cout << "2" << std::endl;
  ASSERT_EQ("test_user", u.username());
  std::cout << "3" << std::endl;
}
