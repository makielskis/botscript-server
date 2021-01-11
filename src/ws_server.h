// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_WS_SERVER_H_
#define BOTSCRIPT_SERVER_WS_SERVER_H_

#include <functional>
#include <memory>

#include "boost/asio.hpp"
#include "boost/asio/io_service.hpp"

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/config/boost_config.hpp"
#include "websocketpp/server.hpp"

#include "rapidjson/document.h"

#include "./bs_server.h"
#include "./conf/ws_server_options.h"
#include "./messages/message.h"

namespace dust {
class key_value_store;
};

namespace botscript_server {

class bs_server_options;

/// Websocket server that uses the bs_server class to respond to requests.
class ws_server {
public:
  /// \param autologin    \see the autologin parameter of bs_server
  /// \param force_proxy  \see the force proxy parameter of bs_server
  /// \param io_service   the Asio I/O service for asynchronous networking
  /// \param store        the key value store for users and configurations
  /// \param packages     the packages to work with
  ws_server(ws_server_options options, bs_server_options mgr_options,
            boost::asio::io_service* io_service,
            std::shared_ptr<dust::key_value_store> store);

  /// Starts listening for connections.
  void start();

  /// Stops the server.
  void stop();

  /// \return the used Asio I/O service
  boost::asio::io_service* io_service() const;

private:
  /// We use the Boost Asio powered websocket server.
  typedef websocketpp::server<websocketpp::config::asio> server;

  /// This callback is used for server activity that does not fit into a
  /// request/response pattern like log messages, status changes, etc.
  ///
  /// This callback should only be called by the bs_server in order to
  /// inform logged in users about activitiy concerning their bots.
  ///
  /// \param sid   the session id to thats associated with this activitiy
  /// \param msgs  the messages to send out
  void on_activity(std::string sid, const std::vector<msg_ptr>& msgs);

  /// This callback will be called on session end.
  ///
  /// \param sid  the session id of the session that ended
  void on_session_end(std::string sid);

  /// Creates a session callback. This is for messages that could open a new
  /// session (register message, login message, user message).
  /// If the session ID is not empty, it will register the session
  /// (= create entries in the sid_con_ map and con_sid_ map).
  ///
  /// \param hdl  the connection handle to send the message response to
  /// \return the session callback
  op_callback create_sid_cb(websocketpp::connection_hdl hdl);

  /// Called when the client closes the websocket.
  /// Informs the bs_server about this fact. The manager again will call the
  /// ws_server::on_session_end() callback.
  ///
  /// \param hdl  the connection that has been closed
  void on_close(websocketpp::connection_hdl hdl);

  /// Called when the client sends a message. Parses the msg and processes it.
  ///
  /// \param hdl  the connection that sends the message
  /// \param msg  the actual message
  void on_msg(websocketpp::connection_hdl hdl, server::message_ptr msg);

  /// Creates the correct operation object from the incoming JSON.
  ///
  /// \param d  the parsed JSON document
  /// \return the operation
  std::shared_ptr<operation> create_op(const rapidjson::Document& d);

  boost::asio::io_service* io_service_;

  boost::asio::signal_set signals_;

  std::unique_ptr<dust::key_value_store> store_;
  bs_server mgr_;

  ws_server_options options_;

  std::map<std::string, websocketpp::connection_hdl> sid_con_map_;
  std::map<websocketpp::connection_hdl, std::string,
           std::owner_less<websocketpp::connection_hdl>>
      con_sid_map_;
  server websocket_server_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_WS_SERVER_H_
