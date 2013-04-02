cmake_minimum_required (VERSION 2.6)
project(botscript-server)

SET (CMAKE_CXX_FLAGS "-std=c++0x")

# shared paths
set(BOTSCRIPT_PATH "/home/simon/development/botscript")

# android paths
set(BOOST_INCLUDE_PATH_ANDR "/home/simon/development/botscript_arm/external_lib/boost/build/include/")
set(BOOST_LIB_PATH_ANDR "/home/simon/development/botscript_arm/external_lib/boost/build/lib/")
set(PUGI_PATH_ANDR "/home/simon/development/botscript_arm/external_lib/pugixml-1.0/src")
set(LUA_PATH_ANDR "/home/simon/development/botscript_arm/external_lib/lua-5.2.0/src")
set(TIDY_PATH_ANDR "/home/simon/development/botscript_arm/external_lib/tidy/include")

set(BOOST_PATH     "/home/simon/development/boost_1_52_0/")
set(PUGI_PATH      "${BOTSCRIPT_PATH}/external_lib/pugixml-1.0/src")
set(LUA_PATH       "${BOTSCRIPT_PATH}/external_lib/lua-5.2.0/src")
set(TIDY_PATH      "${BOTSCRIPT_PATH}/external_lib/tidy/include")

set(BOTSCRIPT_SOURCES
  "${BOTSCRIPT_PATH}/src/http/url.cc"
  "${BOTSCRIPT_PATH}/src/http/http_source.cc"
  "${BOTSCRIPT_PATH}/src/http/http_con.cc"
  "${BOTSCRIPT_PATH}/src/http/util.cc"
  "${BOTSCRIPT_PATH}/src/http/webclient.cc"
  "${BOTSCRIPT_PATH}/src/lua/lua_connection.cc"
  "${BOTSCRIPT_PATH}/src/lua/lua_util.cc"
  "${BOTSCRIPT_PATH}/src/lua/lua_http.cc"
  "${BOTSCRIPT_PATH}/src/bot_browser.cc"
  "${BOTSCRIPT_PATH}/src/module.cc"
  "${BOTSCRIPT_PATH}/src/package.cc"
  "${BOTSCRIPT_PATH}/src/bot.cc"
)

set(HTTP_SERVER_SOURCES
  external_lib/http_server/mime_types.cpp
  external_lib/http_server/reply.cpp
  external_lib/http_server/request_parser.cpp
  external_lib/http_server/server.cpp
)

# regular lib
add_library(botscript-server SHARED
  ${HTTP_SERVER_SOURCES}
  ${BOTSCRIPT_SOURCES}
  src/contents.cpp
  src/botserver_handler.cc
  src/message.cc
  src/service.cc
  src/net_makielski_botscript_BotscriptServer.cpp
)

# android lib
add_library(botscript-server-andr SHARED
  ${HTTP_SERVER_SOURCES}
  ${BOTSCRIPT_SOURCES}
  src/contents.cpp
  src/botserver_handler.cc
  src/message.cc
  src/service.cc
  src/net_makielski_botscript_BotscriptServer.cpp
)

# regular exe (no android)
add_executable(botscript-server-exe
  ${HTTP_SERVER_SOURCES}
  ${BOTSCRIPT_SOURCES}
  src/contents.cpp
  src/botserver_handler.cc
  src/message.cc
  src/service.cc
  src/main.cc
)

# shared include paths
set_property(TARGET botscript-server-andr botscript-server botscript-server-exe PROPERTY INCLUDE_DIRECTORIES
  "${BOTSCRIPT_PATH}/src"
  "../external_lib/websocketpp/src"
  "../external_lib/rapidjson-r64/include"
  "../external_lib/http_server"
)

# android only include paths
set_property(TARGET botscript-server-andr APPEND PROPERTY INCLUDE_DIRECTORIES
  ${BOOST_INCLUDE_PATH_ANDR}
  ${PUGI_PATH_ANDR}
  ${LUA_PATH_ANDR}
  ${TIDY_PATH_ANDR}
)

set_property(TARGET botscript-server-exe botscript-server APPEND PROPERTY INCLUDE_DIRECTORIES
  ${BOOST_PATH}
  ${PUGI_PATH}
  ${LUA_PATH}
  ${TIDY_PATH}
  "/usr/lib/jvm/java-6-openjdk-amd64/include/"
)

# android lib target
set_target_properties(botscript-server-andr PROPERTIES LINK_FLAGS "-L../lib_android -L${BOTSCRIPT_PATH} -L${BOOST_LIB_PATH_ANDR}")
set_target_properties(botscript-server-andr PROPERTIES COMPILE_FLAGS "-DANDROID")
target_link_libraries(botscript-server-andr
  boost_system
  boost_thread
  boost_date_time
  boost_filesystem
  boost_iostreams
  z
  websocketpp
  boost_regex
  tidy
  pugixml
  lua
  dl
  log
)

# regular lib target
set_target_properties(botscript-server PROPERTIES LINK_FLAGS "-L../lib_x86 -L${BOTSCRIPT_PATH} -L${BOOST_PATH}stage/lib/")
target_link_libraries(botscript-server
  boost_system
  boost_thread
  boost_date_time
  boost_filesystem
  boost_iostreams
  z
  websocketpp
  boost_regex
  tidy
  pugixml
  lua
  dl
  -pthread
)

# regular exe (no android)
set_target_properties(botscript-server-exe PROPERTIES LINK_FLAGS "-L../lib_x86 -L${BOOST_PATH}/stage/lib")
target_link_libraries(botscript-server-exe
  boost_system
  boost_thread
  boost_date_time
  boost_filesystem
  boost_iostreams
  z
  websocketpp
  boost_regex
  tidy
  pugixml
  lua
  dl
  -pthread
)
