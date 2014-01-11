#include "botscript_server.h"

#include <memory>

#include "boost/thread.hpp"
#include "boost/filesystem.hpp"

#include "dust/storage/cached_db.h"

#include "../bs_server_options.h"
#include "../ws_server_options.h"
#include "../ws_server.h"

namespace bss = botscript_server;

struct server {
public:
  server(std::string packages_path, const std::string& db_path)
      : s(bss::ws_server_options("127.0.0.1", "9003"),
          bss::bs_server_options(false, true, std::move(packages_path), true),
          &ios, std::make_shared<dust::cached_db>(db_path)) {
  }
  boost::asio::io_service ios;
  bss::ws_server s;
  boost::thread t;
};

void* create_server(const char* arg0) {
  using boost::filesystem::path;
  std::string exe_dir = path(arg0).parent_path().generic_string();
  std::string packages_path = std::string(exe_dir) + "/../Resources/packages";
  std::string db_path = std::string(exe_dir) + "/db";
  return new server(packages_path, db_path);
}

void start_server(void* data) {
  server* s = reinterpret_cast<server*>(data);
  s->s.start();
  s->t = boost::thread(([&ios]() { ios.run(); }));
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
