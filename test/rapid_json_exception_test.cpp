#include "gtest/gtest.h"

#include "../src/messages/account/login/password_login_msg.h"

#define CORRECT_MESSAGE \
"{"\
"  \"type\": ["\
"    \"account\","\
"    \"login\","\
"    \"password\""\
"  ],"\
"  \"arguments\": {"\
"    \"username\": \"testuser\","\
"    \"password\": \"testpassword\""\
"  }"\
"}"

#define INVALID_JSON_MESSAGE \
"{"\
"  \"type\": ["\
"    \"account\","\
"    \"login\","\
"    \"password\""\
"  ],"\
"  \"arguments\": {"\
"    \"username\": \"testuser\""\
"    \"password\": \"testpassword\""\
"  }"\
"}"

#define MISSING_ATTRIBUTE_MESSAGE \
"{"\
"  \"type\": ["\
"    \"account\","\
"    \"login\","\
"    \"password\""\
"  ],"\
"  \"arguments\": {"\
"    \"password\": \"testpassword\""\
"  }"\
"}"

using namespace botscript_server;

TEST(rapid_json_exception_test, success_test) {
  EXPECT_NO_THROW({
    rapidjson::Document d;
    d.Parse<0>(CORRECT_MESSAGE);
    password_login_msg msg(d);
  });
}

TEST(rapid_json_exception_test, invalid_json_test) {
  EXPECT_THROW({
    rapidjson::Document d;
    d.Parse<0>(INVALID_JSON_MESSAGE);
    password_login_msg msg(d);
  }, rapidjson_exception);
}

TEST(rapid_json_exception_test, missing_attribute_test) {
  EXPECT_THROW({
    rapidjson::Document d;
    d.Parse<0>(MISSING_ATTRIBUTE_MESSAGE);
    password_login_msg msg(d);
  }, rapidjson_exception);
}
