#include "gtest/gtest.h"

#include <iostream>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

class user {
public:
  user(std::string name)
      : name_(std::move(name)) {
  }

  std::string name() const {
    return name_;
  }

  bool operator<(const user& u) const {
    return name_ < u.name_;
  }

private:
  std::string name_;
};

class session_info {
public:
  session_info(std::string session_id, user u)
      : session_id(std::move(session_id)),
        user(u) {
  }

  bool operator<(const session_info& o) {
    return session_id < o.session_id;
  }

  std::string session_id;
  user user;
};

using namespace boost::multi_index;

struct id{};
struct name{};

typedef multi_index_container<
  session_info,
  indexed_by<
    ordered_unique<tag<id>, BOOST_MULTI_INDEX_MEMBER(session_info, user, user)>,
    ordered_unique<tag<name>, BOOST_MULTI_INDEX_MEMBER(session_info, std::string, session_id)>
  >
> session_set;

TEST(multi_index_test, access_test) {
  session_set sessions;
  sessions.insert({ "a", user("user1") });
  sessions.insert({ "a", user("user1") });
  sessions.insert({ "a", user("user2") });
  sessions.insert({ "b", user("user1") });
  sessions.insert({ "c", user("user2") });

  typedef session_set::index<id>::type session_set_by_sid;
  for (const auto& session : sessions.get<id>()) {
    std::cout << "[" << session.session_id << ", " << session.user.name() << "]" << std::endl;
  }

  typedef session_set::index<name>::type session_set_by_name;
  session_set_by_name::iterator a = sessions.get<name>().find("a");
  if (a != sessions.get<name>().end()) {
    std::cout << "a found -> " << a->user.name() << std::endl;
  } else {
    std::cout << "a not found" << std::endl;
  }
}
