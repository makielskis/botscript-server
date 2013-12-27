// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_USER_H_
#define BOTSCRIPT_SERVER_USER_H_

#include <string>
#include <vector>
#include <memory>
#include <ctime>

#include "dust/document.h"

#define EXPIRE (60 * 60 * 24)

namespace botscript {
class bot_config;
}

namespace botscript_server {

/// The user object provides a API to a persistent user entity.
///
/// A user is basically represented by:
/// \code{json}
/// {
///   users: {
///     "${username}": {
///       "password": "${md5_hash(${user_pw})}",
///       "email": "${email_address}",
///       "session": {
///         "id": "${md5_hash(${pw_md5_hash} + ${milliseconds})}",
///         "expire": "${milliseconds}"
///       },
///       "bots": {
///         "${bot_identifier}: ${bot_configuration}, ...
///       }
///     }
///   }
/// }
/// \endcode
class user {
 public:
  /// Creates a user object from an existing document.
  /// Throws user_not_found if the document does not contain a valid user.
  user(dust::document doc);

  /// Creates a new persistent user.
  /// Throws username_already_taken if the user already exists in the database.
  ///
  /// \param doc       the document to store this user in
  /// \param password  the password of this user
  user(dust::document doc,
       const std::string& password,
       const std::string& email);

  /// \return whether the bot exists in the database
  bool exists() const;

  /// \return the username of this user entity
  std::string username() const;

  /// \return the email address of this user
  std::string email() const;

  /// \return the users bot configurations
  std::vector<std::shared_ptr<botscript::bot_config>> bot_configs() const;

  /// Removes bot specified by the identifier from the bot list.
  ///
  /// \param identifier  the identifier of the bot to remove
  void remove_bot(const std::string& identifier);

  /// \param identifier  the identifier of the bot to check for
  /// \return whether the user ownes a bot with the given identifier
  bool has_bot(const std::string& identifier) const;

  /// \param identifier  the identifier of the bot get the configuration for
  /// \return the bot configuration
  std::shared_ptr<botscript::bot_config> bot_config(const std::string& identifier) const;

  /// \return the users current session id
  std::string session_id() const;

  /// \return the point in time (Unix timestamp) when the session expires
  std::time_t session_expire() const;

  /// \return true if the session is not expired yet
  bool session_active() const;

  /// Creates a new session ID and sets the expiry timestamp.
  void new_session();

  /// Increments the session expiry timestamp.
  void renew_session();

  /// Sets the new password if the old password was correct, otherwise throws
  /// an password_wrong error.
  ///
  /// \param old_pw  the old password to check (before setting the new)
  /// \param new_pw  the new password to set (if the old password was correct)
  void password(const std::string& old_pw, const std::string& new_pw);

  /// Sets the email address if the old password was correct, otherwise throws
  /// an password_wrong error.
  ///
  /// \param pw         the old password to check (before setting the new)
  /// \param new_email  the new email address to set
  void email(const std::string& pw, const std::string& new_email);

  /// \param password  the password to check
  /// \return true if the password did match the users password
  bool check_password(const std::string& password);

  /// Deletes the whole user.
  void remove();

  /// Less than comparison operator.
  /// Does the comparison based on the unique username.
  ///
  /// \param u  the user to compare with
  inline bool operator<(const user& u) const {
    return username() < u.username();
  }

 private:
  /// The database document containing the stored user information.
  dust::document doc_;
};

}  // namespace botscript_server

#endif  // BOTSERVER_USER_H_
