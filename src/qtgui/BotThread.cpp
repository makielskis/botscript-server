// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "BotThread.h"

#include "dust/storage/cached_db.h"

BotThread::BotThread(botscript_server::ws_server_options wss_options,
                     botscript_server::bs_server_options bss_options)
    : s_(std::move(wss_options), std::move(bss_options), &io_service,
         std::make_shared<dust::cached_db>("db")) {
}

BotThread::~BotThread() {
}

void BotThread::run() {
  s_.start();
}

void BotThread::stopService() {
  std::cout << "Stopping\n";
  s_.stop();
}
