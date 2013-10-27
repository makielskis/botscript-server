// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./net_makielski_botscript_BotscriptServer.h"

#include <jni.h>

#include "../ws_server.h"

#include "../storage/file_config_store.h"
#include "../storage/desktop_user_store.h"

/// Extracts a handle to a bot wrapped in the Bot JNI Java class.
///
/// \param env the JNI environment pointer
/// \param obj the JNI Bot object
botscript_server::ws_server* get_handle(JNIEnv* env, jobject obj) {
  // Get class reference.
  jclass cls = env->GetObjectClass(obj);

  // Get service field in the BotscriptServer object.
  jfieldID fid = env->GetFieldID(cls, "service", "J");
  if (fid == nullptr) {
    std::cerr << "ERROR: service field is null\n";
    return nullptr;
  }

  // Extract, cast and return the bot stored bot handle.
  return reinterpret_cast<botscript_server::ws_server*>(env->GetLongField(obj, fid));
}

extern "C" {

JNIEXPORT void JNICALL Java_net_makielski_botscript_BotscriptServer_start
  (JNIEnv* env, jobject obj) {
  botscript_server::ws_server* s = get_handle(env, obj);
  if (s == nullptr) {
    std::cerr << "ERROR: start not possible, object not initialized\n";
    return;
  }
  s->start("127.0.0.1", "9003");
  delete s;
}

JNIEXPORT void JNICALL Java_net_makielski_botscript_BotscriptServer_stop
  (JNIEnv* env, jobject obj) {
  botscript_server::ws_server* s = get_handle(env, obj);
  if (s == nullptr) {
    std::cerr << "ERROR: stop not possible, object not initialized\n";
    return;
  }
  s->stop();
}

JNIEXPORT jlong JNICALL Java_net_makielski_botscript_BotscriptServer_createService
  (JNIEnv* env, jclass, jstring jpath) {
  const char* cpath = env->GetStringUTFChars(jpath, 0);
  std::string path = cpath;
  env->ReleaseStringUTFChars(jpath, cpath);

  std::unique_ptr<botscript_server::desktop_user_store> ustore(new botscript_server::desktop_user_store(path + "/bot_list.txt"));
  std::unique_ptr<botscript_server::file_config_store> cstore(new botscript_server::file_config_store(path + "/configs"));
  botscript_server::ws_server* s = new botscript_server::ws_server(std::move(cstore), std::move(ustore));
  return reinterpret_cast<long>(s);
}

}
