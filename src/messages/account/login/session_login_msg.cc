
sid_login_message::sid_login_message(const json::Document& doc) {
  sid_ = doc["arguments"]["sid"].GetString();
}

bool sid_login_message::fits(const json::Document& doc) {
  return std::string(doc["type"].GetString()) == "login"
         && doc["arguments"].HasMember("sid")
         && doc["arguments"]["sid"].IsString();
}
