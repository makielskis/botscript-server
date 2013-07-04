// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include <QtGui>

#include "./Window.h"
#include "./BotThread.h"

int main(int argc, char *argv[]) {
  Q_INIT_RESOURCE(systray);

  QApplication app(argc, argv);
  QApplication::setQuitOnLastWindowClosed(false);

  BotThread server;
  server.start();

  Window window(server);

  return app.exec();
}
