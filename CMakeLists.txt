cmake_minimum_required (VERSION 2.6)
project(botscript-server)

add_definitions("-std=c++0x") 

set(BOOST_PATH     "/home/wasabi/boost_1_52_0/")
set(BOTSCRIPT_PATH "/home/wasabi/Development/cppbot/botscript_new")
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

add_library(botscript-server SHARED
  ${HTTP_SERVER_SOURCES}
  ${BOTSCRIPT_SOURCES}
  src/contents.cpp
  src/botserver_handler.cc
  src/message.cc
  src/service.cc
  src/net_makielski_botscript_BotscriptServer.cpp
)

add_executable(botscript-server-exe
  ${HTTP_SERVER_SOURCES}
  ${BOTSCRIPT_SOURCES}
  src/contents.cpp
  src/botserver_handler.cc
  src/message.cc
  src/service.cc
  src/main.cc
)

include_directories(${BOOST_PATH})
include_directories(${PUGI_PATH})
include_directories(${LUA_PATH})
include_directories(${TIDY_PATH})
include_directories("${BOTSCRIPT_PATH}/src")
include_directories("external_lib/websocketpp/src")
include_directories("external_lib/rapidjson-r64/include")
include_directories("external_lib/http_server")
include_directories("/usr/lib/jvm/java-6-openjdk-amd64/include/")

set_target_properties(botscript-server PROPERTIES LINK_FLAGS "-L../ -L${BOTSCRIPT_PATH} -L${BOOST_PATH}stage/lib/")
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

set_target_properties(botscript-server-exe PROPERTIES LINK_FLAGS "-L../ -L${BOTSCRIPT_PATH} -L${BOOST_PATH}stage/lib/")
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
