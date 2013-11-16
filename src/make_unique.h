// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MAKE_UNIQUE_H_
#define BOTSCRIPT_SERVER_MAKE_UNIQUE_H_

namespace botscript_server {

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace botscript_server


#endif  // BOTSCRIPT_SERVER_MAKE_UNIQUE_H_
