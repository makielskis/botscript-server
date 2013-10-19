// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_BS_SERVER_H_
#define BOTSCRIPT_SERVER_BS_SERVER_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <functional>

#include "dust/storage/key_value_store.h"

#include "./messages/message.h"
#include "./operations/operation.h"

namespace boost { namespace asio {
class io_service;
}}
namespace botscript {
class bot;
}

namespace botscript_server {

typedef std::unique_ptr<message> msg_ptr;
typedef std::function<void (std::string, std::vector<msg_ptr>)> sid_callback;
typedef std::function<void (std::string)> session_end_cb;

/// Main bot and user management class.
class bs_server {
 public:
  /// \param io_service            the io_service used to create the bots
  /// \param store                 the storage to use (bot configs + users)
  /// \param packages              the bot packages to provide
  /// \param activity_cb           callback for bot activity (log, status upd.)
  /// \param session_end_callback  callback to inform about session end
  bs_server(boost::asio::io_service* io_service,
            dust::key_value_store& store,
            std::vector<std::string> packages,
            sid_callback activity_cb,
            session_end_cb session_end_callback);

  /// Called on connection close.
  /// Removes all data that is kept for this session.
  void handle_connection_close(const std::string& sid);

  /// Loads the bots that are located in the store.
  void load_bots(std::function<void ()> on_finish);

  /// Calls shutdown on all bots.
  void stop();

 private:
  /// Callback for bots that have been loaded.
  ///
  /// \param bot          the bot that has been loaded
  /// \param err          the load result (empty string = success)
  /// \param configs_ptr  pointer to the remaining configurations
  void on_bot_load(
      std::shared_ptr<botscript::bot> bot,
      std::string err,
      std::shared_ptr<std::vector<std::string>> configs_ptr);

  /// Operations are allowed to change attributes.
  friend class operation;

  /// Asio I/O service required to create bots.
  boost::asio::io_service* io_service_;

  /// Storage for bot configurations and user accounts.
  dust::key_value_store& store_;

  /// Bot package list.
  std::vector<std::string> packages_;

  /// This callback gets called if a bot sends activity (log msg./status upd.).
  sid_callback activity_cb_;

  /// This callback gets called if a session ended:
  ///   - disconnect
  ///   - user logout
  ///   - user account deletion
  session_end_cb session_end_cb_;

  /// Bot map from bot identifier to bot shared pointer.
  std::map<std::string, std::shared_ptr<botscript::bot>> bots_;

  /// Map from session ID to a set of bot identifiers.
  /// This is required to remove the activity callbacks once the user has
  /// disconnected.
  std::map<std::string, std::set<std::string>> sid_bot_ids_map_;

  /// Keeps the bot names that get currently created.
  /// This way no bot can get created twice.
  std::set<std::string> bot_creation_blocklist_;

  /// Keeps the users who are creating bots.
  /// This way no user can create 2 (or more) bots at the same time.
  std::set<std::string> user_bot_creation_blocklist_;
};

}  // namespace botscript_server

#endif  // BOTSERVER_ERROR_H_
