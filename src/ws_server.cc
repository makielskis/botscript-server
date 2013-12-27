// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./ws_server.h"

#include <memory>
#include <signal.h>
#include <iostream>
#include <functional>

#include "rapidjson_with_exception.h"

#include "./messages/message.h"
#include "./messages/failure_msg.h"
#include "./operations/all_ops.h"
#include "./error.h"

using websocketpp::connection_hdl;
using websocketpp::lib::bind;
using websocketpp::lib::error_code;

namespace botscript_server {

ws_server::ws_server(bool force_proxy,
                     std::unique_ptr<dust::key_value_store> store,
                     std::vector<std::string> packages)
    : signals_(io_service_),
      mgr_(force_proxy,
           &io_service_,
           std::move(store),
           std::move(packages),
           std::bind(&ws_server::on_activity, this,
                     std::placeholders::_1,
                     std::placeholders::_2),
           std::bind(&ws_server::on_session_end, this,
                     std::placeholders::_1)) {
  websocket_server_.set_access_channels(websocketpp::log::alevel::none);
  websocket_server_.set_close_handler(bind(&ws_server::on_close, this,
                                           websocketpp::lib::placeholders::_1));
  websocket_server_.set_message_handler(bind(&ws_server::on_msg, this,
                                             websocketpp::lib::placeholders::_1,
                                             websocketpp::lib::placeholders::_2));

  signals_.add(SIGINT);
  signals_.async_wait([this](boost::system::error_code /*ec*/, int /*signo*/) {
    std::cout << "Caught SIGINT, shutting down ..." << std::endl;
    mgr_.stop();
    websocket_server_.stop();
  });
}

void ws_server::start(const std::string& host, const std::string& port) {
  websocket_server_.init_asio(&io_service_);
  websocket_server_.listen(host, port);
  websocket_server_.start_accept();
  websocket_server_.run();
}

void ws_server::stop() {
  websocket_server_.stop();
}

void ws_server::on_activity(std::string sid,
                            std::vector<msg_ptr> msgs) {
  const auto it = sid_con_map_.find(sid);
  if (it != sid_con_map_.cend()) {
    // Connection active, send messagewebsocket_server_.
    for (const auto& msg : msgs) {
      error_code e;
      websocket_server_.send(it->second, msg->to_json(),
                             websocketpp::frame::opcode::TEXT, e);

      // Check error.
      if (e) {
        // Inform bs_server about broken connection
        // and stop processing further messagewebsocket_server_.
        mgr_.handle_connection_close(sid);
        break;
      }
    }
  } else {
    mgr_.handle_connection_close(sid);
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

sid_callback ws_server::create_sid_cb(connection_hdl hdl) {
  return [=](std::string sid, std::vector<msg_ptr> msgs) {
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
    // Deregister session in bs_server.
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

  // Inform bs_server if it was a registered session.
  if (it != con_sid_map_.end()) {
    mgr_.handle_connection_close(it->second);
  }
}

void ws_server::on_msg(connection_hdl hdl, server::message_ptr msg) {
  std::cout << "--> " << msg->get_payload() << std::endl;

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

  // Create operation.
  std::shared_ptr<operation> op;
  try {
    op = create_op(d);
  } catch(const rapidjson_exception& e) {
    std::cerr << "could not parse message \"" << msg->get_payload() << "\" "
              << "(expected attributes missing)\n";
  } catch(const boost::system::system_error& e) {
    std::cerr << "could not parse message \"" << msg->get_payload() << "\" "
              << "(unknown type)\n";
    return;
  }

  // Execute operation.
  try {
    std::vector<msg_ptr> msgs = op->execute(mgr_, create_sid_cb(hdl));

    // Send response.
    for (const auto& msg : msgs) {
      error_code send_ec;
      websocket_server_.send(hdl, msg->to_json(),
                             websocketpp::frame::opcode::TEXT, send_ec);
    }
  } catch(const boost::system::system_error& e) {
    failure_msg m(0, op->type(), e.code().value(), e.code().message());
    error_code send_ec;
    websocket_server_.send(hdl, m.to_json(),
                           websocketpp::frame::opcode::TEXT, send_ec);
  }
}

std::shared_ptr<operation> ws_server::create_op(const rapidjson::Document& d) {
  if (d["type"].Size() < 1) {
    throw boost::system::system_error(botscript_server::error::invalid_message);
  }

  std::string type = d["type"][rapidjson::SizeType(0)].GetString();

  if (type == "register") {
    return std::make_shared<register_op>(d);
  } else if (type == "login") {
    return std::make_shared<login_op>(d);
  } else if (type == "user") {
    if (d["type"].Size() == 1u) {
      return std::make_shared<user_op>(d);
    } else if (d["type"].Size() >= 2u) {
      type = d["type"][1].GetString();

      if (type == "bot" && d["type"].Size() >= 3u) {
        type = d["type"][2].GetString();

        if (type == "create" && d["type"].Size() >= 4u) {
          type = d["type"][3].GetString();
          if (type == "new") {
            return std::make_shared<create_new_bot_op>(d);
          } else if (type == "reactivate") {
            return std::make_shared<reactivate_bot_op>(d);
          }
        } else if (type == "delete") {
          return std::make_shared<delete_bot_op>(d);
        } else if (type == "execute") {
          return std::make_shared<execute_bot_op>(d);
        }
      } else if (type == "update" && d["type"].Size() >= 3u) {
        type = d["type"][2].GetString();

        if (std::string(d["type"][2].GetString()) == "delete") {
          return std::make_shared<delete_update_op>(d);
        } else if (type == "email") {
          return std::make_shared<email_update_op>(d);
        } else if (type == "password") {
          return std::make_shared<password_update_op>(d);
        }
      }
    }
  }

  throw boost::system::system_error(botscript_server::error::invalid_message);
}

}  // namespace botscript_server
