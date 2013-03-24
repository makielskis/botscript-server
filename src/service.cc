#include "service.h"

#include <boost/asio/io_service.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/function.hpp>

#include <server.hpp>
#include <request.hpp>
#include <reply.hpp>
#include <mime_types.hpp>

#include "./botserver_handler.h"
#include "./contents.h"

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

void service::start(short ws_port, short http_port) {
  std::cerr << "service::start(" << ws_port << ", " << http_port << ")";
  std::cout << "loading contents\n";
  std::map<std::string, std::string> content = contents::load();
  std::cerr << "starting ..\n";
  std::string http_port_str = std::to_string(http_port);
  try {
    // Instatiate handler and Web Socket server endpoint.
    bot_server_handler* bsh = new bot_server_handler();
    server::handler::ptr handler(bsh);
    server endpoint(handler);
    bsh->io_service(endpoint.io_service());

    // Create filehandler.
    using http_server::request;
    using http_server::reply;
    typedef boost::function<void(const request&, reply&)> http_handler;
    http_handler h = [this, &content](const request& req, reply& rep) {
      // Decode url to path.
      std::string request_path;
      if (!url_decode(req.uri, request_path))
      {
        rep = reply::stock_reply(reply::bad_request);
        return;
      }

      // If path ends in slash (i.e. is a directory) then add "index.html".
      if (request_path[request_path.size() - 1] == '/')
      {
        request_path += "index.html";
      }

      // Determine the file extension.
      std::size_t last_slash_pos = request_path.find_last_of("/");
      std::size_t last_dot_pos = request_path.find_last_of(".");
      std::string extension;
      if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
      {
        extension = request_path.substr(last_dot_pos + 1);
      }

      // Find response content.
      std::string sha1 = sha1sum(request_path.c_str(), request_path.length());
      const auto it = content.find(sha1);
      if (it == content.end())
      {
        rep = reply::stock_reply(reply::not_found);
        return;
      }

      // Write response.
      rep.status = reply::ok;
      rep.content = it->second;
      rep.headers.resize(3);
      rep.headers[0].name = "Content-Length";
      rep.headers[0].value = std::to_string(rep.content.size());
      rep.headers[1].name = "Content-Type";
      rep.headers[1].value = http_server::mime_types::extension_to_type(extension);
      rep.headers[2].name = "Content-Encoding";
      rep.headers[2].value = "gzip";
    };

    http_server::server(*endpoint.io_service(), "127.0.0.1", http_port_str, h)();
    io_service_ = endpoint.io_service();
    endpoint.listen(ws_port);
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}

}  // namespace botscript_server
