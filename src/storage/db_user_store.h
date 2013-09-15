// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DB_USER_STORE_H_
#define DB_USER_STORE_H_

#include "user_store.h"

#include <string>
#include <set>

#include "boost/asio/io_service.hpp"

#include "./kc_wrapper.h"
#include "./callback.h"

namespace botscript_server {

/// Implementation of a user_store based on kyoto db.
class db_user_store : public user_store {
 public:
  /// Create/open a kyotodb stored in the given path.
  db_user_store(const std::string& path,
                boost::asio::io_service* io_service);

  /// \param io_service  the io_service to set
  void io_service(boost::asio::io_service* io_service);

  virtual void add_user(const std::string& username,
                        const std::string& password,
                        const std::string& email,
                        cb<std::string>::type cb) override;

  virtual void remove_user(const std::string& session_id,
                           const std::string& password,
                           empty_cb cb) override;

  virtual void login(const std::string& username,
                     const std::string& password,
                     cb<std::string>::type cb) override;

  virtual void logout(const std::string& session_id,
                      empty_cb cb) override;

  virtual void set_password(const std::string& session_id,
                            const std::string& old_password,
                            const std::string& new_password,
                            empty_cb cb) override;

  virtual void get_email(const std::string& session_id,
                         cb<std::string>::type cb) override;

  virtual void set_email(const std::string& session_id,
                         const std::string& password,
                         const std::string& email,
                         empty_cb cb) override;

  virtual void get_bots(const std::string& session_id,
                        cb<std::vector<std::string>>::type cb) override;

  virtual void add_bot(const std::string& session_id,
                       const std::string& identifier,
                       empty_cb cb) override;

  virtual void remove_bot(const std::string& session_id,
                          const std::string& identifier,
                          empty_cb cb) override;

  const std::unordered_map<std::string, std::string>& sid_user();

  const std::unordered_map<std::string, std::string>& user_sid();

  const std::unordered_map<std::string, time_t>& sid_expire();

 private:
  /// Splits the botlist at ","
  ///
  /// \param  the string to split
  /// \return the splitted string
  std::set<std::string> split(const std::string& s);

  /// Creates a comma seperated string from the given string vector
  ///
  /// \param  the string vector to turn to a comma seperated string
  /// \return the comma seperated string
  std::string to_string(const std::set<std::string>& s);

  /// Generate a session id and update the session mappings for the username.
  ///
  /// Postcondition:
  ///   - The session mappings (sid_user_, user_sid_) have entries for the given
  ///     username
  ///
  /// \param  username the username to update the mappings with
  /// \return the generated session id
  std::string make_session(const std::string& username);

  /// Check if the given session id is valid and extend its vadility if so.
  bool check_session(const std::string& sid);

  /// Service managing I/O operations.
  boost::asio::io_service* io_service_;

  /// Kyoto Cabinet wrapper handle.
  db db_;

  /// DB default entry for users
  entry users_;

  /// Map session id to username.
  std::unordered_map<std::string, std::string> sid_user_;

  /// Map username to sessions.
  std::unordered_map<std::string, std::string> user_sid_;

  /// Map session to expire timestamp.
  std::unordered_map<std::string, time_t> sid_expire_;
};

}  // namespace botscript_server

#endif  // DB_USER_STORE_H_
