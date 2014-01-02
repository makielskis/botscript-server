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

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/ordered_index.hpp"
#include "boost/multi_index/identity.hpp"
#include "boost/multi_index/member.hpp"

#include "boost/asio/signal_set.hpp"

#include "dust/storage/key_value_store.h"
#include "dust/document.h"

#include "./user.h"
#include "./session.h"
#include "./operations/operation.h"
#include "./messages/message.h"
#include "./bs_server_options.h"

namespace boost { namespace asio {
class io_service;
}}
namespace botscript {
class bot;
class bot_config;
}

namespace botscript_server {

typedef std::unique_ptr<message> msg_ptr;
typedef std::function<void (std::string, std::vector<msg_ptr>)> sid_callback;
typedef std::function<void (std::string)> session_end_cb;
typedef std::function<void (std::string, std::string, std::string)> update_cb;
typedef std::shared_ptr<botscript::bot_config> bot_config_ptr;

/// Used as index tag for session multi-set.
struct index_session_id {
};

/// Used as index tag for session multi-set.
struct index_user {
};

/// Data structure holding session information.
/// Indexed in both directions: username <-> session ID
typedef boost::multi_index_container<
  session,
  boost::multi_index::indexed_by<
    boost::multi_index::ordered_unique<
      boost::multi_index::tag<index_user>,
      BOOST_MULTI_INDEX_MEMBER(session, user, u)
    >,
    boost::multi_index::ordered_unique<
      boost::multi_index::tag<index_session_id>,
      BOOST_MULTI_INDEX_MEMBER(session, std::string, id)
    >
  >
> session_set;

/// Main bot and user management class.
class bs_server {
 public:
  /// \param io_service            the io_service used to create the bots
  /// \param store                 the storage to use (bot configs + users)
  /// \param packages              the bot packages to provide
  /// \param activity_cb           callback for bot activity (log, status upd.)
  /// \param session_end_callback  callback to inform about session end
  bs_server(bs_server_options options,
            boost::asio::io_service* io_service,
            std::shared_ptr<dust::key_value_store> store,
            sid_callback activity_cb,
            session_end_cb session_end_callback);

  /// Starts listening for SIGUSR1 and reloads the packages on this signal.
  void listen_for_update_packages_signal();

  /// Calls bot::load_packages with the path given in the constructor and
  /// updates the packages_ vector with the JSON package descriptions.
  void update_packages();

  /// Called on connection close.
  /// Removes all data that is kept for this session.
  void handle_connection_close(const std::string& sid);

  /// Loads the specified bot configuration and removes it from the configs vec.
  ///
  /// \param configs  the configurations
  /// \param index    the index in the configurations vector
  void load_bot(std::shared_ptr<std::vector<bot_config_ptr>> configs,
                std::size_t index);

  /// Loads the bots that are located in the store.
  void load_bots();

  /// Calls shutdown on all bots.
  void stop();

  /// Updates the users session in the session multi set.
  ///
  /// \param u  the user to update the session for
  void update_session(const user& u);

  /// Private attributes and methods --->

  /// Callback for bots that have been loaded.
  ///
  /// \param bot          the bot that has been loaded
  /// \param err          the load result (empty string = success)
  /// \param configs_ptr  pointer to the remaining configurations
  void on_bot_load(std::shared_ptr<botscript::bot> bot,
                   std::string err,
                   std::shared_ptr<std::vector<bot_config_ptr>> configs);

  /// Callback for activity from bots whose user is currently connected.
  ///
  /// \param sid  the session id of the user to send this message to
  update_cb sid_cb(const std::string& sid);

  /// Callback for activity from bots whose user is not connected.
  update_cb print_cb();

  /// Returns the logs of the bots of the specified user.
  ///
  /// \return map with (key = bot identifier), (value = bot log)
  std::map<std::string, std::string> bot_logs(const user& u) const;

  /// Configuration.
  bs_server_options options_;

  /// Asio I/O service required to create bots.
  boost::asio::io_service* io_service_;

  /// Signals set for reloading the packages on USERSIG1.
  boost::asio::signal_set signals_;

  /// Storage for bot configurations and user accounts.
  std::shared_ptr<dust::key_value_store> store_;

  /// Multimap storing the sessions together with the user.
  session_set sessions_;

  /// The document holding all users.
  dust::document users_;

  /// Bot package list.
  std::vector<std::string> packages_;

  /// This callback gets called if a bot sends activity (log msg/status upd.).
  sid_callback activity_cb_;

  /// This callback gets called when a session ended:
  ///   - disconnect
  ///   - user logout
  ///   - user account deletion
  session_end_cb session_end_cb_;

  /// Bot map from bot identifier to bot shared pointer.
  std::map<std::string, std::shared_ptr<botscript::bot>> bots_;

  /// Keeps the users who are creating bots.
  /// This way no user can create more than one bot at the same time.
  std::set<std::string> user_bot_creation_blocklist_;
};

}  // namespace botscript_server

#endif  // BOTSERVER_ERROR_H_
