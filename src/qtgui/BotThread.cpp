// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "BotThread.h"

#include "dust/storage/cached_db.h"

BotThread::BotThread(const std::string& exe_dir)
    : s_(false, exe_dir, &io_service,
         std::make_shared<dust::cached_db>("db")) {
}

BotThread::~BotThread() {
}

void BotThread::run() {
  s_.start("127.0.0.1", "9003");
}

void BotThread::stopService() {
  s_.stop();
}
