// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSERVER_BOT_THREAD_H_
#define BOTSERVER_BOT_THREAD_H_

#include <QThread>

#include "../service.h"

class QThread;

class BotThread : public QThread {

  Q_OBJECT

 public:
  void stopService();

 private:
  void run();

  botscript_server::service s;
};

#endif  // BOTSERVER_BOT_THREAD_H_
