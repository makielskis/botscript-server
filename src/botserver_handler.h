// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSERVER_H_
#define BOTSERVER_H_

#include <memory>

#include <boost/asio/io_service.hpp>

#include "websocketpp.hpp"

#include "bot.h"
#include "file_config_store.h"

namespace botscript_server {

/// Class that handles incoming client messages by implementing the
/// websocketpp::server::server::handler interface.
class bot_server_handler : public websocketpp::server::handler {
 public:
  /// \param io_service the Asio io_service object for the bots
  void io_service(boost::asio::io_service* io_service);

  /// Validates the connection before it is accepted.
  /// Time to check session status, authentification, ...
  ///
  /// \param con the incoming connection
  void validate(websocketpp::server::connection_ptr con);

  /// Called after the connection has been established.
  ///
  /// \param con the incoming connection
  void on_open(websocketpp::server::connection_ptr con);

  /// Called when a client disconnects.
  ///
  /// \param con the incoming connection
  void on_close(websocketpp::server::connection_ptr con);

  /// Called on an incloming client message.
  ///
  /// \param con the incoming connection
  void on_message(websocketpp::server::connection_ptr con,
                  message_ptr msg);

  /// Sends the message to the client.
  ///
  /// \param msg the message to send
  void send(const std::string& msg);

  /// Sends an update message to the client.
  ///
  /// \param i the identifier
  /// \param k key
  /// \param v the new value
  void callback(std::string i, std::string k, std::string v);

  /// Sends all bots to the client.
  void send_bots();

 private:
  boost::asio::io_service* io_service_;
  websocketpp::server::connection_ptr connection_;
  std::map<std::string, std::shared_ptr<botscript::bot>> bots_;
  std::string packages_;
  botscript_server::file_config_store config_store_;
};

}  // namespace botscript_server

#endif  // BOTSERVER_H_
