// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef BOTSCRIPT_SERVER_MAKE_UNIQUE_H_
#define BOTSCRIPT_SERVER_MAKE_UNIQUE_H_

#include <memory>

namespace botscript_server {

#ifndef _MSC_VER
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#else
using std::make_unique;
#endif

}  // namespace botscript_server


#endif  // BOTSCRIPT_SERVER_MAKE_UNIQUE_H_
