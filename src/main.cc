#include <iostream>

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include "rapidjson_with_exception.h"

#include "./storage/db_config_store.h"
#include "./storage/db_user_store.h"
#include "./bot_manager.h"
#include "./messages/outgoing_msgs.h"

using namespace botscript_server;

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using websocketpp::lib::ref;

msg_callback create_cb(server& s, connection_hdl hdl) {
  return [&s, hdl](std::vector<outgoing_msg_ptr> msgs) {
    for (const auto& msg : msgs) {
      s.send(hdl, msg->to_json(), websocketpp::frame::opcode::TEXT);
    }
  };
}

void default_on_msg(server& s, bot_manager& mgr, connection_hdl hdl, server::message_ptr msg) {
  rapidjson::Document d;
  bool failure = d.Parse<0>(msg->get_payload().c_str()).HasParseError();
  if (failure) {
    std::cerr << "could not parse message \"" << msg->get_payload() << "\" "
              << "(no valid JSON message)\n";
    return;
  }

  msg_callback cb = create_cb(s, hdl);
  try {
    std::string type = d["type"][rapidjson::SizeType(0)].GetString();

    if (type == "register") {
      mgr.handle_register_msg(register_msg(d), cb);
    } else if (type == "login") {
      mgr.handle_login_msg(login_msg(d), cb);
    } else if (type == "user") {
      if (d["type"].Size() == 1u) {
        mgr.handle_user_msg(user_msg(d), cb);
      } else {
        type = d["type"][1].GetString();

        if (type == "bot") {
          type = d["type"][2].GetString();

          if (type == "create") {
            mgr.handle_create_bot_msg(create_bot_msg(d), cb);
          } else if (type == "delete") {
            mgr.handle_delete_bot_msg(delete_bot_msg(d), cb);
          } else if (type == "execute") {
            mgr.handle_execute_bot_msg(execute_bot_msg(d), cb);
          } else if (type == "reactivate") {
            mgr.handle_reactivate_bot_msg(reactivate_bot_msg(d), cb);
          }
        } else if (type == "update") {
          type = d["type"][2].GetString();

          if (std::string(d["type"][2].GetString()) == "delete") {
            mgr.handle_delete_update_msg(delete_update_msg(d), cb);
          } else if (type == "email") {
            mgr.handle_email_update_msg(email_update_msg(d), cb);
          } else if (type == "password") {
            mgr.handle_password_update_msg(password_update_msg(d), cb);
          }
        }
      }
    }
  } catch(const rapidjson_exception& e) {
    std::cerr << "could not parse message \"" << msg->get_payload() << "\" "
              << "(expected attributes missing)\n";
  }
}

int main() {
  boost::asio::io_service io_service;

  db_config_store config_store("./config_store.kch", &io_service);
  db_user_store user_store("./user_store.kch", &io_service);
  bot_manager manager(config_store, user_store, &io_service);

  server s;
  s.set_message_handler(bind(&default_on_msg, ref(s), ref(manager), ::_1,::_2));

  s.init_asio(&io_service);
  s.listen(9003);
  s.start_accept();

  s.run();
}
