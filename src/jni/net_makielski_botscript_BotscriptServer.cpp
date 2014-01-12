// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./net_makielski_botscript_BotscriptServer.h"

#include <jni.h>

#include "../c_api/botscript_server.h"

/// Extracts a handle to a bot wrapped in the Bot JNI Java class.
///
/// \param env the JNI environment pointer
/// \param obj the JNI Bot object
/// \return the server object
void* get_handle(JNIEnv* env, jobject obj) {
  // Get class reference.
  jclass cls = env->GetObjectClass(obj);

  // Get service field in the BotscriptServer object.
  jfieldID fid = env->GetFieldID(cls, "service", "J");
  if (fid == nullptr) {
    return nullptr;
  }

  // Extract, cast and return the bot stored bot handle.
  return reinterpret_cast<void*>(env->GetLongField(obj, fid));
}

JNIEXPORT void JNICALL Java_net_makielski_botscript_BotscriptServer_start
  (JNIEnv* env, jobject obj) {
  void* handle = get_handle(env, obj);
  if (handle == nullptr) {
    return;
  }
  start_server(handle);
}

JNIEXPORT void JNICALL Java_net_makielski_botscript_BotscriptServer_stop
  (JNIEnv* env, jobject obj) {
  void* handle = get_handle(env, obj);
  if (handle == nullptr) {
    return;
  }
  stop_server(handle);
}

JNIEXPORT jlong JNICALL Java_net_makielski_botscript_BotscriptServer_createService
  (JNIEnv* env, jclass, jstring dbPath) {
  const char* db_path = env->GetStringUTFChars(dbPath, 0);
  long handle = reinterpret_cast<long>(create_server(db_path));
  env->ReleaseStringUTFChars(dbPath, db_path);
  return handle;
}
