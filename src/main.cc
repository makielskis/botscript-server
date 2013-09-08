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
using websocketpp::lib::error_code;

server* server_ptr = nullptr;
std::map<std::string, connection_hdl> sid_con_map;
std::map<connection_hdl, std::string> con_sid_map;

void activity_cb(std::string sid, std::vector<outgoing_msg_ptr> msgs) {
  const auto it = sid_con_map.find(sid);
  if (it != sid_con_map.cend()) {
    // Connection active, send messages.
    for (const auto& msg : msgs) {
      error_code e;
      server_ptr->send(it->second, msg->to_json(), websocketpp::frame::opcode::TEXT, e);

      // Check error (break on error).
      if (e) {
        std::cerr << "[ERROR] unable to send messages: " << e.message() << "\n";
        break;
      }
    }
  } else {
    std::cerr << "[FATAL] message for unknown session " << sid << " not delivered\n";
  }
}

void on_session_end(std::string sid) {
  const auto sid_con_it = sid_con_map.find(sid);
  if (sid_con_it != sid_con_map.end()) {
    const auto con_sid_it = con_sid_map.find(sid_con_it->second);
    if (con_sid_it != con_sid_map.end()) {
      con_sid_map.erase(con_sid_it);
    }
    sid_con_map.erase(sid_con_it);
  }
  std::cout << "Session " << sid << " ended\n";
}

msg_callback create_cb(server& s, connection_hdl hdl) {
  return [&s, hdl](std::vector<outgoing_msg_ptr> msgs) {
    for (const auto& msg : msgs) {
      error_code err;
      s.send(hdl, msg->to_json(), websocketpp::frame::opcode::TEXT, err);
      if (err) {
        std::cout << "[ERROR] could not deliver message\n";
      }
    }
  };
}

sid_callback create_sid_cb(server& s, connection_hdl hdl) {
  return [&s, hdl](std::string sid, std::vector<outgoing_msg_ptr> msgs) {
    if (!sid.empty()) {
      sid_con_map[sid] = hdl;
    }

    for (const auto& msg : msgs) {
      error_code err;
      s.send(hdl, msg->to_json(), websocketpp::frame::opcode::TEXT, err);
      if (err) {
        std::cout << "[ERROR] could not deliver message\n";
      }
    }
  };
}

void on_close(bot_manager& mgr, connection_hdl hdl) {
  const auto it = con_sid_map.find(hdl);
  if (it != con_sid_map.end()) {
    mgr.handle_connection_close(it->second);
  }
}

void on_msg(server& s, bot_manager& mgr, connection_hdl hdl, server::message_ptr msg) {
  rapidjson::Document d;
  bool failure = d.Parse<0>(msg->get_payload().c_str()).HasParseError();
  if (failure) {
    std::cerr << "could not parse message \"" << msg->get_payload() << "\" "
              << "(no valid JSON message)\n";
    return;
  }

  try {
    std::string type = d["type"][rapidjson::SizeType(0)].GetString();

    if (type == "register") {
      mgr.handle_register_msg(register_msg(d), create_sid_cb(s, hdl));
    } else if (type == "login") {
      mgr.handle_login_msg(login_msg(d), create_sid_cb(s, hdl));
    } else if (type == "user") {
      if (d["type"].Size() == 1u) {
        mgr.handle_user_msg(user_msg(d), create_sid_cb(s, hdl));
      } else {
        msg_callback cb = create_cb(s, hdl);
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
  sid_callback cb = std::bind(&activity_cb, std::placeholders::_1, std::placeholders::_2);
  std::function<void (std::string)> session_end_cb = std::bind(&on_session_end, std::placeholders::_1);
  bot_manager manager("./packages", config_store, user_store, std::move(cb), session_end_cb, &io_service);

  server s;
  server_ptr = &s;
  s.set_close_handler(bind(&on_close, ref(manager), ::_1));
  s.set_message_handler(bind(&on_msg, ref(s), ref(manager), ::_1,::_2));

  manager.load_bots([&]() {
    std::cout << "Finished loading bots\n";
  });


  s.init_asio(&io_service);
  s.listen(9003);
  s.start_accept();
  s.run();
}
