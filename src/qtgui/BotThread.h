// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSERVER_BOT_THREAD_H_
#define BOTSERVER_BOT_THREAD_H_

#include <string>

#include <QThread>

#include "../ws_server.h"

class QThread;

class BotThread : public QThread {

  Q_OBJECT

 public:
  BotThread(const std::string& exe_dir);
  virtual ~BotThread();
  void stopService();

 private:
  void run();

  boost::asio::io_service io_service;
  botscript_server::ws_server s_;
};

#endif  // BOTSERVER_BOT_THREAD_H_
