#ifndef BOTSCRIPT_SERVER_H_
#define BOTSCRIPT_SERVER_H_

extern "C" {
void* create_server(const char* exe_dir);
void start_server(void* server);
void stop_server(void* server);
void close_server(void* server);
}

#endif  // BOTSCRIPT_SERVER_H_
