// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./net_makielski_botscript_BotscriptServer.h"

#include <jni.h>

#include "./service.h"

using namespace botscript_server;

/// Extracts a handle to a bot wrapped in the Bot JNI Java class.
///
/// \param env the JNI environment pointer
/// \param obj the JNI Bot object
service* get_handle(JNIEnv* env, jobject obj) {
  // Get class reference.
  jclass cls = env->GetObjectClass(obj);

  // Get service field in the BotscriptServer object.
  jfieldID fid = env->GetFieldID(cls, "service", "J");
  if (fid == nullptr) {
    std::cerr << "ERROR: service field is null\n";
    return nullptr;
  }

  // Extract, cast and return the bot stored bot handle.
  return reinterpret_cast<service*>(env->GetLongField(obj, fid));
}

JNIEXPORT void JNICALL Java_net_makielski_botscript_BotscriptServer_start
  (JNIEnv* env, jobject obj) {
  service* s = get_handle(env, obj);
  if (s == nullptr) {
    std::cerr << "ERROR: start not possible, object not initialized\n";
    return;
  }
  s->start(9003);
}

JNIEXPORT void JNICALL Java_net_makielski_botscript_BotscriptServer_stop
  (JNIEnv* env, jobject obj) {
  service* s = get_handle(env, obj);
  if (s == nullptr) {
    std::cerr << "ERROR: stop not possible, object not initialized\n";
    return;
  }
  s->stop();
}

JNIEXPORT jlong JNICALL Java_net_makielski_botscript_BotscriptServer_createService
  (JNIEnv* env, jobject obj) {
  service* s = new service();
  return reinterpret_cast<long>(s);
}
