// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_USER_H_
#define BOTSCRIPT_SERVER_USER_H_

#include <string>
#include <vector>
#include <memory>

#include "dust/document.h"

namespace botscript {
class bot_config;
}

namespace botscript_server {

/// The user object provides a API to a persistent user entity.
class user {
 public:
  /// Creates a user object from an existing document.
  /// Throws user_not_found if the document does not contain a valid user.
  user(dust::document doc);

  /// Creates a new peristent user.
  /// Throws username_already_taken if the user already exists in the database.
  ///
  /// \param doc       the document to store this user in
  /// \param password  the password of this user
  user(dust::document doc,
       const std::string& password);

  /// \return the username of this user entity
  std::string username();

  /// Sets the new password if the old password was correct, otherwise throws
  /// an password_wrong error.
  ///
  /// \param old_pw  the old password to check (before setting the new)
  /// \param new_pw  the new password to set (if the old password was correct)
  void password(const std::string& old_pw, const std::string& new_pw);

  /// \param password  the password to check
  /// \return true if the password did match the users password
  bool check_password(const std::string& password);

  /// \return the users bot configurations
  std::vector<std::shared_ptr<botscript::bot_config>> bots_config();

  dust::document bots_document();

 private:
  dust::document doc_;
};

}  // namespace botscript_server

#endif  // BOTSERVER_USER_H_
