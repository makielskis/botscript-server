#ifndef BOTSCRIPT_SERVER_H_
#define BOTSCRIPT_SERVER_H_

extern "C" {
__attribute__ ((visibility ("default"))) void* create_server(const char* exe_dir);
__attribute__ ((visibility ("default"))) void start_server(void* server);
__attribute__ ((visibility ("default"))) void stop_server(void* server);
}

#endif  // BOTSCRIPT_SERVER_H_
