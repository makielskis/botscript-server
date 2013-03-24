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

add_executable(botscript-server
  ${BOTSCRIPT_SOURCES}
  src/main.cc
  src/botserver_handler.cc
  src/message.cc
)

include_directories(${BOOST_PATH})
include_directories(${PUGI_PATH})
include_directories(${LUA_PATH})
include_directories(${TIDY_PATH})
include_directories("${BOTSCRIPT_PATH}/src")
include_directories("external_lib/websocketpp/src")
include_directories("external_lib/rapidjson-r64/include")

set_target_properties(botscript-server PROPERTIES LINK_FLAGS "-L../ -L${BOTSCRIPT_PATH} -L${BOOST_PATH}stage/lib/")
target_link_libraries(botscript-server
  boost_system
  boost_filesystem
  boost_iostreams
  z
  boost_thread -pthread
  websocketpp
  boost_regex
  boost_date_time
  tidy
  pugixml
  lua
  dl
)
