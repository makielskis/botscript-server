// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./net_makielski_botscript_BotscriptServer.h"

#include <memory>

#include <jni.h>

#include "dust/storage/cached_db.h"

#include "../bs_server_options.h"
#include "../ws_server_options.h"
#include "../ws_server.h"

namespace bss = botscript_server;

/// Extracts a handle to a bot wrapped in the Bot JNI Java class.
///
/// \param env the JNI environment pointer
/// \param obj the JNI Bot object
bss::ws_server* get_handle(JNIEnv* env, jobject obj) {
  // Get class reference.
  jclass cls = env->GetObjectClass(obj);

  // Get service field in the BotscriptServer object.
  jfieldID fid = env->GetFieldID(cls, "service", "J");
  if (fid == nullptr) {
    std::cerr << "ERROR: service field is null\n";
    return nullptr;
  }

  // Extract, cast and return the bot stored bot handle.
  return reinterpret_cast<bss::ws_server*>(env->GetLongField(obj, fid));
}

JNIEXPORT void JNICALL Java_net_makielski_botscript_BotscriptServer_start
  (JNIEnv* env, jobject obj) {
  bss::ws_server* s = get_handle(env, obj);
  if (s == nullptr || s->io_service() == nullptr) {
    std::cerr << "ERROR: start not possible, object not initialized\n";
    return;
  }
  s->start();
  s->io_service()->run();
}

JNIEXPORT void JNICALL Java_net_makielski_botscript_BotscriptServer_stop
  (JNIEnv* env, jobject obj) {
  auto s = get_handle(env, obj);
  if (s == nullptr) {
    std::cerr << "ERROR: stop not possible, object not initialized\n";
    return;
  }
  s->stop();
  if (s->io_service() != nullptr) {
    delete s->io_service();
  }
  delete s;
}

JNIEXPORT jlong JNICALL Java_net_makielski_botscript_BotscriptServer_createService
  (JNIEnv* env, jclass, jstring dbPath) {
  const char* c_db_path = env->GetStringUTFChars(dbPath, 0);
  std::string db_path = c_db_path;
  env->ReleaseStringUTFChars(dbPath, c_db_path);

  auto ios = new boost::asio::io_service();
  auto s = new bss::ws_server(bss::ws_server_options("127.0.0.1", "9003"),
                              bss::bs_server_options(false, true, "", true),
                              ios, std::make_shared<dust::cached_db>(db_path));
  return reinterpret_cast<long>(s);
}
