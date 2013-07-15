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
  /// \param host the host to listen on (e.g. "localhost" or "0.0.0.0")
  /// \param service the port to listen on (e.g. "http" or "80")
  void start(const std::string& host, const std::string& service);

  /// Stops the service.
  void stop();

 private:

  /// Points to the io_service currently in use.
  boost::asio::io_service* io_service_;
};

}  // namespace botscript-server

#endif  // BOTSCRIPT_SERVER_SERVICE
