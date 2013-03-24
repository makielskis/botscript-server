#ifndef BOTSCRIPT_SERVER_SERVICE
#define BOTSCRIPT_SERVER_SERVICE

#include <iostream>
#include <string>

#include <boost/asio/io_service.hpp>

namespace botscript_server {

/// This service starts an HTTP server serving files that were "compiled" in
/// the contents.h / contents.cpp file.
/// Additionally it handles Web Socket connections enabling to control
/// (create/delete/configure) bots.
class service {
 public:
  /// Initializes the io_service with NULL.
  service() : io_service_(nullptr) {
    std::cerr << "service::service()\n";
  }

  /// Starts the server.
  ///
  /// \param ws_port the web socket port to listen on
  /// \param http_port the HTTP port to listen on
  void start(short ws_port, short http_port);

  /// Stops the service again.
  void stop();

 private:
  /// Utility function to calculate the SHA1 checksum in HEX format.
  ///
  /// \param data   the data to hash
  /// \param count  the amount of data to hash
  /// \return the SHA1 checksum
  static std::string sha1sum(const void *data, std::size_t count);

  /// URL decodes the given string and writes the decoded string to out
  ///
  /// \param in   the in string to decode
  /// \param out  the decoded string
  /// \return whether the decoding was successful
  static bool url_decode(const std::string& in, std::string& out);

  /// Points to the io_service currently in use.
  boost::asio::io_service* io_service_;
};

}  // namespace botscript-server

#endif  // BOTSCRIPT_SERVER_SERVICE
