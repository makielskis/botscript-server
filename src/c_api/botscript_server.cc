#include "botscript_server.h"

#include <memory>

#include "boost/thread.hpp"

#include "dust/storage/cached_db.h"

#include "../bs_server_options.h"
#include "../ws_server_options.h"
#include "../ws_server.h"

namespace bss = botscript_server;

struct server {
public:
  server()
      : s(bss::ws_server_options("127.0.0.1", "9003"),
          bss::bs_server_options(
              false, true, "/Users/felix/Code/bot/botscript-server/packages", true),
          &ios, std::make_shared<dust::cached_db>("/Users/felix/Code/bot/botscript-server/db")) {
  }

  boost::asio::io_service ios;
  bss::ws_server s;
  boost::thread t;
};

void* create_server() {
  return new server();
}

void start_server(void* data) {
  server* s = reinterpret_cast<server*>(data);
  s->t = boost::thread([s]() { s->s.start(); });
}

void close_server(void* data) {
  server* s = reinterpret_cast<server*>(data);
  delete s;
}

void stop_server(void* data) {
  server* s = reinterpret_cast<server*>(data);
  s->s.stop();
  s->t.join();
}
