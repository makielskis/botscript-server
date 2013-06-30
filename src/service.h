#ifndef BOTSCRIPT_SERVER_SERVICE
#define BOTSCRIPT_SERVER_SERVICE

#include <iostream>
#include <string>

#include <boost/asio/io_service.hpp>

namespace botscript_server {

/// This service handles Web Socket connections enabling to control
/// (create/delete/configure) bots.
class service {
 public:
  /// Initializes the io_service with NULL.
  service();

  /// Starts the server.
  ///
  /// \param ws_port the web socket port to listen on
  /// \param http_port the HTTP port to listen on
  void start(short ws_port);

  /// Stops the service.
  void stop();

 private:

  /// Points to the io_service currently in use.
  boost::asio::io_service* io_service_;
};

}  // namespace botscript-server

#endif  // BOTSCRIPT_SERVER_SERVICE
