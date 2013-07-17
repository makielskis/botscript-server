// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "./botserver_handler.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "./message.h"
#include "./storage/file_config_store.h"

namespace json = rapidjson;
namespace bs = botscript;

namespace botscript_server {

bot_server_handler::bot_server_handler(config_store& config_store)
  : config_store_(config_store) {
}

void bot_server_handler::io_service(boost::asio::io_service* io_service) {
  io_service_ = io_service;
  std::string packages = "[";
  std::vector<std::string> package_list = bs::bot::load_packages("./packages");
  if (package_list.size() != 0) {
    for (const std::string& p : package_list) {
      packages += p + ",";
    }
    packages = packages.substr(0, packages.length() - 1);
  }
  packages += "]";
  packages_ = "{\
                \"type\": \"packages\",\
                \"arguments\": " + packages + "\
              }";

  for (const std::string& config : config_store_.get_all()) {
    std::shared_ptr<bs::bot> b = std::make_shared<bs::bot>(io_service_);
    b->callback_ = std::bind(&bot_server_handler::callback, this,
                             std::placeholders::_1,
                             std::placeholders::_2,
                             std::placeholders::_3);
    b->init(config, [this](std::shared_ptr<bs::bot> b, std::string err){
      if (err.empty()) {
        bots_[b->identifier()] = b;
      } else {
        b->shutdown();
        auto remove_cb = [b](config_store::error_indicator e) {
          if (e) {
            std::cout << "ERROR: " << b->identifier() << " couldn't be loaded"
                      << " and config could not be removed\n";
          }
        };
        config_store_.remove(b->identifier(), remove_cb);
      }
    });
  }
}

void bot_server_handler::validate(websocketpp::server::connection_ptr con) {
}

void bot_server_handler::on_open(websocketpp::server::connection_ptr con) {
  connection_ = con;
  send(packages_);
}

void bot_server_handler::on_close(websocketpp::server::connection_ptr con) {
  connection_.reset();
}

void bot_server_handler::on_message(websocketpp::server::connection_ptr con,
                                    message_ptr msg) {
  // Parse in JSON message and check message format:
  // {
  //   'type': [String],
  //   'arguments': [Object]
  // }
  json::Document doc;
  if (doc.Parse<0>(msg->get_payload().c_str()).HasParseError()
      || !doc.IsObject()
      || !doc.HasMember("type") || !doc["type"].IsString()
      || !doc.HasMember("arguments") || !doc["arguments"].IsObject()) {
    std::cout << "\"" << msg->get_payload() << "\"doesn't fit message format\n";
    return;
  }

  // Create message if it fits.
  if (sid_login_message::fits(doc)) {
    send_bots();
  } else if (create_message::fits(doc)) {
    create_message m = create_message(doc);
    std::shared_ptr<bs::bot> b = std::make_shared<bs::bot>(io_service_);
    b->callback_ = std::bind(&bot_server_handler::callback, this,
                             std::placeholders::_1,
                             std::placeholders::_2,
                             std::placeholders::_3);
    b->init(m.config(), [this](std::shared_ptr<bs::bot> b, std::string err){
      if (err.empty()) {
        config_store_.add(b, [this, b](config_store::error_indicator e) {
          if (!e) {
            bots_[b->identifier()] = b;
            send_bots();
          } else {
            b->shutdown();
            send("{\
                    \"type\": \"account\",\
                    \"arguments\": {\
                      \"key\": \"create_bot\",\
                      \"success\": \"storage error\"\
                    }\
                  }");
          }
        });
      } else {
        b->shutdown();
        send("{\
                \"type\": \"account\",\
                \"arguments\": {\
                  \"key\": \"create_bot\",\
                  \"success\": \"" + err + "\"\
                }\
              }");
      }
    });
  } else if (delete_message::fits(doc)) {
    delete_message m = delete_message(doc);
    const auto it = bots_.find(m.identifier());
    if (it == bots_.end()) {
      std::cout << "bot \"" << m.identifier() << "\" not found\n";
      return;
    }
    it->second->shutdown();
    bots_.erase(it);

    auto cb = [m] (config_store::error_indicator e) {
      if (e) {
        std::cout << "couldn't delete config for " << m.identifier() << "\n";
      }
    };
    config_store_.remove(m.identifier(), cb);

    send_bots();
  } else if (execute_message::fits(doc)) {
    execute_message m = execute_message(doc);
    const auto it = bots_.find(m.identifier());
    if (it == bots_.end()) {
      std::cout << "ERROR: bot \"" << m.identifier() << "\" not found\n";
      return;
    }
    it->second->execute(m.command(), m.argument());
  } else {
    std::cout << "ERROR: couldn't parse \"" << msg->get_payload() << "\"\n";
    return;
  }
}

void bot_server_handler::send(const std::string& msg) {
  if (connection_ != connection_ptr()) {
    connection_->send(msg);
  }
}

void bot_server_handler::callback(std::string i, std::string k, std::string v) {
  if (k == "log") {
    std::cout << v << std::flush;
  } else {
    const auto it = bots_.find(i);
    if (it != bots_.end()) {
      const auto seperator_pos = k.find('_');
      const std::string module(k.substr(0, seperator_pos));
      const std::string option(k.substr(seperator_pos + 1, k.length()));

      auto cb = [i] (config_store::error_indicator e) {
        if (e) {
          std::cout << "couldn't update config for " << i << "\n";
        }
      };
      config_store_.update_attribute(it->second, module, option, v, cb);
    }
  }

  // Remove trailing new line and send update to the client.
  v = (k == "log" ? v.substr(0, v.length() - 1) : v);
  send("{\
          \"type\": \"update\",\
          \"arguments\": {\
            \"identifier\": \"" + i + "\",\
            \"key\": \"" + k + "\",\
            \"value\": \"" + v + "\"\
          }\
        }");
}

void bot_server_handler::send_bots() {
  std::string bots = "{\"type\": \"bots\", \"arguments\": {";
  if (bots_.size() != 0) {
    for (const auto& b : bots_) {
      bots += "\"" + b.first + "\":";
      bots += b.second->configuration(false) + ",";
    }
    bots = bots.substr(0, bots.length() - 1);
  }
  bots += "}}";
  send(bots);
}

}  // namespace botscript_server
