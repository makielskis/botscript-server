// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_CONFIG_H_
#define BOTSCRIPT_SERVER_CONFIG_H_

#if defined(DESKTOP)

#include "dust/storage/mem_store.h"
#define KEY_VALUE_STORE  dust::mem_store

#else

#include "dust/storage/mem_store.h"
#define FORCE_PROXY
#define KEY_VALUE_STORE  dust::mem_store

#endif

#endif  // BOTSCRIPT_SERVER_CONFIG_H_
