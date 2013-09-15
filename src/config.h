// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_CONFIG_H_
#define BOTSCRIPT_SERVER_CONFIG_H_

#if defined(DESKTOP)

#include "./storage/file_config_store.h"
#include "./storage/desktop_user_store.h"
#define CONFIG_STORE(IO) file_config_store("./configs", &IO)
#define USER_STORE(IO)   desktop_user_store("./bot_list.txt")

#else

#include "./storage/db_config_store.h"
#include "./storage/db_user_store.h"
#define FORCE_PROXY
#define USER_STORE(IO)   db_user_store("./user_store.kch", &IO)
#define CONFIG_STORE(IO) db_config_store("./config_store.kch", &IO)

#endif

#endif  // BOTSCRIPT_SERVER_CONFIG_H_
