#include "./service.h"

using namespace botscript_server;

int main() {
  service s;
  s.start(9003, 8080);
  return 0;
}
