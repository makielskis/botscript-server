// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef CONFIG_STORE_H
#define CONFIG_STORE_H

#include <string>
#include <vector>
#include <functional>
#include <exception>
#include <memory>

#include "boost/optional.hpp"

#include "bot.h"

#include "./callback.h"

namespace botscript_server {

/// User storage adapter interface.
///
/// For instance there could be:
///   - a database implementation
///   - an in memory implementation
///   - a file based implementation
///
/// In case of a failure (i.e. I/O operation failed) the callback will
/// provide an exception (type std::exception) contained in the error_indicator.
///
/// When implementing: use salted hashs to store the passwords!
class user_store {
 public:
  /// Virtual destructor for inheriting classes.
  virtual ~user_store() {
  }

  /// Adds a user with the specified properties to the database.
  ///
  /// Precondition:
  ///   - No user with given username exists
  ///
  /// Postcondidion:
  ///   - User with given username, password, email and session ID exists
  ///
  /// \param username  the username of the new user
  /// \param password  the password (plain text, not hashed) of the new user
  /// \param email     the email address of the new user
  /// \param cb        callback that will be called on finish
  ///                  parameter: string - the session id for the new user
  virtual void add_user(const std::string& username,
                        const std::string& password,
                        const std::string& email,
                        cb<std::string>::type cb) = 0;

  /// Remove user with given username from the store.
  ///
  /// Precondition:
  ///   - User with given session_id exists
  ///   - Given password matches the users password
  ///
  /// Postconditioin:
  ///   - The user does not exist
  ///
  /// \param username  the username of the user that should be removed
  /// \param password  the current password of the user that should be removed
  /// \param cb        callback that will be called on finish
  virtual void remove_user(const std::string& session_id,
                           const std::string& password,
                           empty_cb cb) = 0;

  /// Checks the password of the user with given username.
  ///
  /// Precondition:
  ///   - User with given username exists
  ///   - Given password matches the users password
  ///
  /// Postcondition:
  ///   - A valid session id is created for the user
  ///
  /// \param username  the username of the user
  /// \param password  the password to check
  /// \param cb        callback that will be called on finish
  ///                  parameter: string - a session id for the user
  virtual void login(const std::string& username,
                     const std::string& password,
                     cb<std::string>::type cb) = 0;

  /// Deletes the specified session.
  ///
  /// Precondition:
  ///   - Given session exists in mappings
  ///
  /// Postcondition:
  ///   - No session with given id exists
  ///
  /// \param session_id  the session id to delete
  /// \param cb          callback the will be called on finish
  virtual void logout(const std::string& session_id, empty_cb cb) = 0;

  /// Sets the new password for the user.
  ///
  /// Precondition:
  ///   - User with given session id exists
  ///   - Given old password machtches the users password
  ///
  /// Postcondition:
  ///   - Users password is new password
  ///
  /// \param username      the username of the user
  /// \param old_password  the current password of the user
  /// \param new_password  the new password to set
  /// \param cb            callback that will be called on finish
  ///                      parameter bool - indicates success of the operation
  ///                      (success = user exists && password correct)
  virtual void set_password(const std::string& session_id,
                            const std::string& old_password,
                            const std::string& new_password,
                            empty_cb cb) = 0;

  /// Looks the email of the specified user up.
  ///
  /// Precondition:
  ///   - User with given session id exists.
  ///
  /// \param username  the username of user
  /// \param cb        callback that will be called on finish
  ///                  parameter: the users email address
  virtual void get_email(const std::string& session_id,
                         cb<std::string>::type cb) = 0;

  /// Sets the new email address for the specified user.
  ///
  /// Precondition:
  ///   - User with given session id exists.
  ///   - Provided password matches the users password.
  ///
  /// Postcondition:
  ///   - Users email is set to the provided email address.
  ///
  /// \param username  the username of the user
  /// \param password  the current password of the user
  /// \param cb        callback that will be called on finish
  ///                  parameter: bool - indicates success of the operation
  ///                  (success = user exists && password correct)
  virtual void set_email(const std::string& session_id,
                         const std::string& password,
                         const std::string& email,
                         empty_cb cb) = 0;

  /// Reads the bot list of the specified user.
  ///
  /// Precondition:
  ///   - User with given session id exists.
  ///
  /// \param username  the username of the user to lookup the bots for
  /// \param cb        callback that will be called on finish
  ///                  parameter: vector of bot identifiers
  virtual void get_bots(const std::string& session_id,
                        cb<std::vector<std::string>>::type cb) = 0;

  /// Adds a bot to the bot list of the user.
  ///
  /// Precondition:
  ///   - User with given session id exists.
  ///   - User with given session id has no bot with given bot identifier
  ///
  /// Postcondition:
  ///   - User with given session id has bot with given bot identifier
  ///
  /// \param username    the username of the user to lookup the bots for
  /// \param identifier  the identifier to add
  /// \param cb          callback that will be called on finish
  virtual void add_bot(const std::string& session_id,
                       const std::string& bot_identifier,
                       empty_cb cb) = 0;

  /// Removes a bot from the bot list of the user.
  ///
  /// Precondition:
  ///   - User with given session id exists.
  ///   - User with given session has bot with given bot_identifier.
  ///
  /// Postcondition:
  ///   - User with given session id has no bot with given bot_identifier.
  ///
  /// \param username    the username of the user
  /// \param identifier  the identifier to remove
  /// \param cb          callback that will be called on finish
  virtual void remove_bot(const std::string& session_id,
                          const std::string& identifier,
                          empty_cb cb) = 0;
};

}  // namespace botscript_server

#endif  // CONFIG_STORE_H
