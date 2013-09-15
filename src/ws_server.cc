// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./ws_server.h"

#include <iostream>
#include <functional>

#include "rapidjson_with_exception.h"

#include "./config.h"

using websocketpp::connection_hdl;
using websocketpp::lib::bind;
using websocketpp::lib::error_code;

namespace botscript_server {

ws_server::ws_server()
    : config_store_(new CONFIG_STORE(io_service_)),
      user_store_(new USER_STORE(io_service_)),
      mgr_("./packages",
           *config_store_.get(),
           *user_store_.get(),
           std::bind(&ws_server::on_activity, this,
                     std::placeholders::_1,
                     std::placeholders::_2),
           std::bind(&ws_server::on_session_end, this,
                     std::placeholders::_1),
           &io_service_) {
  websocket_server_.set_close_handler(bind(&ws_server::on_close, this,
                                           websocketpp::lib::placeholders::_1));
  websocket_server_.set_message_handler(bind(&ws_server::on_msg, this,
                                             websocketpp::lib::placeholders::_1,
                                             websocketpp::lib::placeholders::_2));
}

void ws_server::start(const std::string& host, const std::string& port) {
  mgr_.load_bots([&]() {
    std::cout << "Finished loading bots\n";
  });

  websocket_server_.init_asio(&io_service_);
  websocket_server_.listen(host, port);
  websocket_server_.start_accept();
  websocket_server_.run();
}

void ws_server::stop() {
  websocket_server_.stop();
}

void ws_server::on_activity(std::string sid,
                            std::vector<outgoing_msg_ptr> msgs) {
  const auto it = sid_con_map_.find(sid);
  if (it != sid_con_map_.cend()) {
    // Connection active, send messagewebsocket_server_.
    for (const auto& msg : msgs) {
      error_code e;
      websocket_server_.send(it->second, msg->to_json(),
                             websocketpp::frame::opcode::TEXT, e);

      // Check error.
      if (e) {
        // Inform manager about broken connection
        // and stop processing further messagewebsocket_server_.
        mgr_.handle_connection_close(sid);
        break;
      }
    }
  } else {
    std::cerr << "[FATAL] message for unknown session "
              << sid << " not delivered\n";
  }
}

void ws_server::on_session_end(std::string sid) {
  // Remove session id from maps.
  const auto sid_con_it = sid_con_map_.find(sid);
  if (sid_con_it != sid_con_map_.end()) {
    const auto con_sid_it = con_sid_map_.find(sid_con_it->second);
    if (con_sid_it != con_sid_map_.end()) {
      con_sid_map_.erase(con_sid_it);
    }
    sid_con_map_.erase(sid_con_it);
  }
}

msg_callback ws_server::create_cb(connection_hdl hdl) {
  return [=](std::vector<outgoing_msg_ptr> msgs) {
    // Send out messages.
    for (const auto& msg : msgs) {
      error_code err;
      websocket_server_.send(hdl, msg->to_json(), websocketpp::frame::opcode::TEXT, err);
      if (err) {
        std::cout << "[ERROR] could not deliver message\n";
        break;
      }
    }
  };
}

sid_callback ws_server::create_sid_cb(connection_hdl hdl) {
  return [=](std::string sid, std::vector<outgoing_msg_ptr> msgs) {
    // Send out messages.
    bool failure = false;
    for (const auto& msg : msgs) {
      error_code err;
      websocket_server_.send(hdl, msg->to_json(),
                             websocketpp::frame::opcode::TEXT, err);
      if (err) {
        std::cout << "[ERROR] could not deliver message\n";
        failure = true;
        break;
      }
    }

    // If the message could not be sent to the user:
    // Deregister session in manager.
    if (failure && !sid.empty()) {
      mgr_.handle_connection_close(sid);
      return;
    }

    // Register session if the session id is set (= successful login)
    if (!sid.empty()) {
      sid_con_map_[sid] = hdl;
      con_sid_map_[hdl] = sid;
    }
  };
}

void ws_server::on_close(connection_hdl hdl) {
  // Find corresponding session id.
  const auto it = con_sid_map_.find(hdl);

  // Inform manager if it was a registered session.
  if (it != con_sid_map_.end()) {
    mgr_.handle_connection_close(it->second);
  }
}

void ws_server::on_msg(connection_hdl hdl, server::message_ptr msg) {
  // Parse JSON message content.
  rapidjson::Document d;
  bool failure = d.Parse<0>(msg->get_payload().c_str()).HasParseError();

  // Check failure:
  // Print error message and return (prevent further message processing)
  if (failure) {
    std::cerr << "could not parse message \"" << msg->get_payload() << "\" "
              << "(no valid JSON message)\n";
    return;
  }

  // Process message:
  // Create message class and hand it over to the manager.
  try {
    std::string type = d["type"][rapidjson::SizeType(0)].GetString();

    if (type == "register") {
      mgr_.handle_register_msg(register_msg(d), create_sid_cb(hdl));
    } else if (type == "login") {
      mgr_.handle_login_msg(login_msg(d), create_sid_cb(hdl));
    } else if (type == "user") {
      if (d["type"].Size() == 1u) {
        mgr_.handle_user_msg(user_msg(d), create_sid_cb(hdl));
      } else {
        msg_callback cb = create_cb(hdl);
        type = d["type"][1].GetString();

        if (type == "bot") {
          type = d["type"][2].GetString();

          if (type == "create") {
            mgr_.handle_create_bot_msg(create_bot_msg(d), cb);
          } else if (type == "delete") {
            mgr_.handle_delete_bot_msg(delete_bot_msg(d), cb);
          } else if (type == "execute") {
            mgr_.handle_execute_bot_msg(execute_bot_msg(d), cb);
          } else if (type == "reactivate") {
            mgr_.handle_reactivate_bot_msg(reactivate_bot_msg(d), cb);
          }
        } else if (type == "update") {
          type = d["type"][2].GetString();

          if (std::string(d["type"][2].GetString()) == "delete") {
            mgr_.handle_delete_update_msg(delete_update_msg(d), cb);
          } else if (type == "email") {
            mgr_.handle_email_update_msg(email_update_msg(d), cb);
          } else if (type == "password") {
            mgr_.handle_password_update_msg(password_update_msg(d), cb);
          }
        }
      }
    }
  } catch(const rapidjson_exception& e) {
    std::cerr << "could not parse message \"" << msg->get_payload() << "\" "
              << "(expected attributes missing)\n";
  }
}

}  // namespace botscript_server
