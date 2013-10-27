// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_DUST_CONFIG_H_
#define BOTSCRIPT_SERVER_DUST_CONFIG_H_

#include "config.h"

#include "dust/document.h"

namespace botscript_server {

class dust_config : public botscript::config {
 public:
  dust_config(dust::document document);
  dust_config(dust::document document, const std::string& json_config);
  dust_config(dust::document document,
              const std::string& identifier,
              const std::string& username,
              const std::string& password,
              const std::string& package,
              const std::string& server,
              const std::map<std::string, botscript::string_map>& module_settings);

  virtual botscript::command_sequence init_command_sequence() const override;
  virtual std::string to_json(bool with_password) const override;
  virtual std::string value_of(const std::string& key) const override;

  virtual void inactive(bool flag) override;
  virtual bool inactive() const override;

  virtual std::string identifier() const override;
  virtual std::string username() const override;
  virtual std::string password() const override;
  virtual std::string package() const override;
  virtual std::string server() const override;
  virtual std::map<std::string, botscript::string_map> module_settings() const override;

  virtual void set(const std::string& module,
                   const std::string& key,
                   const std::string& value) override;
  virtual void set(const std::string& key,
                   const std::string& value) override;

 private:
  dust::document doc_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_DUST_CONFIG_H_
