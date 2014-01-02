// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSERVER_WINDOW_H_
#define BOTSERVER_WINDOW_H_

#include <QSystemTrayIcon>
#include <QDialog>
#include <QAction>
#include <QMenu>

#include "./BotThread.h"

class QAction;
class QMenu;
class QDialog;
class QCloseEvent;

class Window : public QDialog {

  Q_OBJECT

 public:
  Window(BotThread& botThread);

 private slots:
  void bot();
  void stop();

 private:
  void createActions();
  void createTrayIcon();

  QAction *botAction;
  QAction *quitAction;
  QAction *versionAction;
  QSystemTrayIcon *trayIcon;
  QMenu *trayIconMenu;

  BotThread& botThread;
};

#endif  // BOTSERVER_WINDOW_H_
