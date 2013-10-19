// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MESSAGES_PACKAGES_MSG_
#define BOTSCRIPT_SERVER_MESSAGES_PACKAGES_MSG_

#include <string>
#include <vector>

#include "./message.h"

namespace botscript_server {

/// Packages message:
/// "These are the available packages"
///
/// {
///   'type': 'packages',
///   'arguments': [Array of Objects]
/// }
class packages_msg : public message {
 public:
  /// \param packages  JSON formatted package descriptions
  packages_msg(const std::vector<std::string>& packages);

  virtual std::string to_json() const override;

  /// \return the packages
  const std::vector<std::string>& packages() const;

 private:
  /// JSON formatted package descriptions
  const std::vector<std::string> packages_;
};

}  // namespace botscript_server

#endif  // BOTSCRIPT_SERVER_MESSAGES_PACKAGES_MSG_
