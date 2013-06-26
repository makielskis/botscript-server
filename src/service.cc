#include "service.h"

#include <boost/asio/io_service.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>

#include <server.hpp>
#include <request.hpp>
#include <reply.hpp>
#include <mime_types.hpp>

#include "./botserver_handler.h"

namespace botscript_server {

namespace http_server = ::http::server4;
using namespace websocketpp;
using namespace botscript_server;

std::string service::sha1sum(const void *data, std::size_t count) {
  boost::uuids::detail::sha1 hasher;
  char hash[20];
  hasher.process_bytes(data, count);
  unsigned int digest[5];
  hasher.get_digest(digest);
  for(int i = 0; i < 5; ++i) {
    const char *tmp = reinterpret_cast<char *>(digest);
    hash[i * 4] = tmp[i * 4 + 3];
    hash[i * 4 + 1] = tmp[i * 4 + 2];
    hash[i * 4 + 2] = tmp[i * 4 + 1];
    hash[i * 4 + 3] = tmp[i * 4];
  }
  std::stringstream res;
  res << std::hex;
  for(int i = 0; i < 20; ++i) {
      res << ((hash[i] & 0x000000F0) >> 4)
          << (hash[i] & 0x0000000F);
  }
  return res.str();
}

bool service::url_decode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i)
  {
    if (in[i] == '%')
    {
      if (i + 3 <= in.size())
      {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value)
        {
          out += static_cast<char>(value);
          i += 2;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else if (in[i] == '+')
    {
      out += ' ';
    }
    else
    {
      out += in[i];
    }
  }
  return true;
}

void service::start(short ws_port) {
  std::cerr << "service::start(" << ws_port << ")";
  std::cerr << "starting ..\n";
  try {
    // Instatiate handler and Web Socket server endpoint.
    bot_server_handler* bsh = new bot_server_handler();
    server::handler::ptr handler(bsh);
    server endpoint(handler);
    bsh->io_service(endpoint.io_service());

    io_service_ = endpoint.io_service();
    endpoint.listen(ws_port);
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}

void service::stop() {
  io_service_->stop();
}

}  // namespace botscript_server
