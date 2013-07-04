// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "BotThread.h"

void BotThread::run() {
  s.start(9003);
}

void BotThread::stopService() {
  s.stop();
}
