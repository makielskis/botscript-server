// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef  BOTSCRIPT_SERVER_WS_SERVER_H_
#define  BOTSCRIPT_SERVER_WS_SERVER_H_

#include <memory>
#include <functional>

#include "boost/asio/io_service.hpp"
#include "boost/asio.hpp"

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include "./bot_manager.h"

#include "./messages/outgoing_msgs.h"

namespace botscript_server {

/// Websocket server that uses the bot_mananger class to respond to requests.
class ws_server {
 public:
  ws_server();

  /// Starts listening for connections.
  void start(const std::string& host, const std::string& port);

  /// Stops the server.
  void stop();

 private:
  typedef websocketpp::server<websocketpp::config::asio> server;

  /// This callback is used for server activity that does not fit into a
  /// request/response pattern like log messages, status changes, etc.
  ///
  /// This callback should only be called by the bot_manager in order to
  /// inform logged in users about activitiy concerning their bots.
  ///
  /// \param sid   the session id to thats associated with this activitiy
  /// \param msgs  the messages to send out
  void on_activity(std::string sid, std::vector<outgoing_msg_ptr> msgs);

  /// This callback will be called on session end.
  ///
  /// \param sid  the session id of the session that ended
  void on_session_end(std::string sid);

  /// Creates a message callback for a general message.
  ///
  /// \param hdl  the connection handle to send the message response to
  msg_callback create_cb(websocketpp::connection_hdl hdl);

  /// Creates a session callback. This is for messages that could open a new
  /// session (register message, login message, user message).
  /// If the session ID is not empty, it will register the session
  /// (= create entries in the sid_con_ map and con_sid_ map).
  ///
  /// \param hdl  the connection handle to send the message response to
  sid_callback create_sid_cb(websocketpp::connection_hdl hdl);

  /// Called when the client closes the websocket.
  /// Informs the manager about this fact.
  ///
  /// \param hdl  the connection that has been closed
  void on_close(websocketpp::connection_hdl hdl);

  /// Called when the client sends a message.
  /// Parses the message and processes it.
  ///
  /// \param hdl  the connection that sends the message
  /// \param msg  the actual message
  void on_msg(websocketpp::connection_hdl hdl, server::message_ptr msg);

  boost::asio::io_service io_service_;

  boost::asio::signal_set signals_;

  std::unique_ptr<config_store> config_store_;
  std::unique_ptr<user_store> user_store_;
  bot_manager mgr_;

  std::map<std::string, websocketpp::connection_hdl> sid_con_map_;
  std::map<websocketpp::connection_hdl, std::string> con_sid_map_;
  server websocket_server_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_WS_SERVER_H_
