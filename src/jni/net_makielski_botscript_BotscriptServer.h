/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class net_makielski_botscript_BotscriptServer */

#ifndef _Included_net_makielski_botscript_BotscriptServer
#define _Included_net_makielski_botscript_BotscriptServer
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     net_makielski_botscript_BotscriptServer
 * Method:    start
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_net_makielski_botscript_BotscriptServer_start
  (JNIEnv *, jobject);

/*
 * Class:     net_makielski_botscript_BotscriptServer
 * Method:    stop
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_net_makielski_botscript_BotscriptServer_stop
  (JNIEnv *, jobject);

/*
 * Class:     net_makielski_botscript_BotscriptServer
 * Method:    createService
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_net_makielski_botscript_BotscriptServer_createService
  (JNIEnv *, jclass, jstring);

#ifdef __cplusplus
}
#endif
#endif
