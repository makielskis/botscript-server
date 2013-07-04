// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include <QtGui>
#include <QDesktopServices>
#include <QUrl>

#include "Window.h"

Window::Window(BotThread& botThread)
  : botThread(botThread) {
  createActions();
  createTrayIcon();
}

void Window::bot() {
  QDesktopServices::openUrl(QUrl("http://makielski.net/mobileui",
                            QUrl::TolerantMode));
}

void Window::stop() {
  botThread.stopService();
  QApplication::quit();
}

void Window::createActions() {
  botAction = new QAction(tr("&Bot"), this);
  connect(botAction, SIGNAL(triggered()), this, SLOT(bot()));

  quitAction = new QAction(tr("&Quit"), this);
  connect(quitAction, SIGNAL(triggered()), this, SLOT(stop()));
}

void Window::createTrayIcon() {
  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(botAction);
  trayIconMenu->addAction(quitAction);

  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayIconMenu);
  trayIcon->setIcon(QIcon(":/invader.svg"));

  trayIcon->show();
}
