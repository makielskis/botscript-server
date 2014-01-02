// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include <QtGui>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>

#include "Window.h"

#include "../botscript_server_version.h"

Window::Window(BotThread& botThread)
    : botThread(botThread) {
  createActions();
  createTrayIcon();
}

void Window::bot() {
  QDesktopServices::openUrl(QUrl(MOBILE_UI_URL, QUrl::TolerantMode));
}

void Window::stop() {
  botThread.stopService();
  botThread.wait();
  QApplication::quit();
}

void Window::createActions() {
  botAction = new QAction(tr("&Bot"), this);
  connect(botAction, SIGNAL(triggered()), this, SLOT(bot()));

  quitAction = new QAction(tr("&Quit"), this);
  connect(quitAction, SIGNAL(triggered()), this, SLOT(stop()));

  versionAction = new QAction(
      tr(std::string(std::string("&v") + botscript_server::version()).c_str()),
      this);
  versionAction->setEnabled(false);
}

void Window::createTrayIcon() {
  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(botAction);
  trayIconMenu->addAction(quitAction);
  trayIconMenu->addAction(versionAction);

  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayIconMenu);
  trayIcon->setIcon(QIcon(":/invader.svg"));

  trayIcon->show();
}
