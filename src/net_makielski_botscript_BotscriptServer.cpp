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
  std::cerr << "getting class reference\n";
  jclass cls = env->GetObjectClass(obj);

  // Get service field in the BotscriptServer object.
  std::cerr << "getting service field\n";
  jfieldID fid = env->GetFieldID(cls, "service", "J");
  if (fid == nullptr) {
    std::cerr << "ERROR: service field is null\n";
    return nullptr;
  }

  // Extract, cast and return the bot stored bot handle.
  std::cerr << "returning field value as service* pointer\n";
  std::cerr << "VALUE: " << env->GetLongField(obj, fid) << "\n";
  return reinterpret_cast<service*>(env->GetLongField(obj, fid));
}

JNIEXPORT void JNICALL Java_net_makielski_botscript_BotscriptServer_start
  (JNIEnv* env, jobject obj) {
  std::cerr << "Java_net_makielski_botscript_BotscriptServer_start\n";
  service* s = get_handle(env, obj);
  std::cerr << "service: " << ((long) s) << "\n";
  if (s == nullptr) {
    std::cerr << "operation not possible, object not initialized\n";
  }
  std::cerr << "handle = " << ((long) s) << "\n";
  s->start(9003);
}

JNIEXPORT void JNICALL Java_net_makielski_botscript_BotscriptServer_stop
  (JNIEnv* env, jobject obj) {
  std::cerr << "Java_net_makielski_botscript_BotscriptServer_stop\n";
  service* s = get_handle(env, obj);
  if (s == nullptr) {
    std::cerr << "operation not possible, object not initialized\n";
  }
  s->stop();
}

JNIEXPORT jlong JNICALL Java_net_makielski_botscript_BotscriptServer_createService
  (JNIEnv* env, jobject obj) {
  std::cerr << "creating service\n";
  service* s = new service();
  return reinterpret_cast<long>(s);
}
