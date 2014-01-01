// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "boost/filesystem.hpp"

#include <QtGui>
#include <QApplication>

#include "./Window.h"
#include "./BotThread.h"

class QApplication;

using boost::filesystem::path;

int main(int argc, char *argv[]) {
  Q_INIT_RESOURCE(systray);

  std::string exe_dir = path(argv[0]).parent_path().generic_string();

  QApplication app(argc, argv);
  app.setOrganizationName("Makielski");
  app.setApplicationName("makielskisbot");
  QApplication::setQuitOnLastWindowClosed(false);

  BotThread server(exe_dir + "/packages");
  server.start();

  Window window(server);

  return app.exec();
}
