#include "./service.h"

using namespace botscript_server;

int main() {
  service s;
  s.start("127.0.0.1", "http");
  return 0;
}
