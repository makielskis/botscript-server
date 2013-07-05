// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include <QtGui>
#include <QApplication>

#include "./Window.h"
#include "./BotThread.h"

class QApplication;

int main(int argc, char *argv[]) {
  Q_INIT_RESOURCE(systray);

  QApplication app(argc, argv);
  app.setOrganizationName("Makielski");
  app.setApplicationName("Bot Script");
  QApplication::setQuitOnLastWindowClosed(false);

  BotThread server;
  server.start();
  std::cout << "starting server ..\n";

  Window window(server);

  return app.exec();
}
