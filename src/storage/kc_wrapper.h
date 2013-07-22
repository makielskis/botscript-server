// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef KC_WRAPPER_
#define KC_WRAPPER_


#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <kcpolydb.h>

namespace kc = kyotocabinet;

namespace botscript_server {

class entry {
 public:
  entry(kc::PolyDB& db,
        const std::string& index)
    : db_(db) {
    path_.push_back(index);
    path_string_ = index;
  }

  entry(kc::PolyDB& db,
        std::vector<std::string> path,
        const std::string& index)
    : db_(db),
      path_(path) {
    path_.push_back(index);

    for (const std::string& path_element : path_) {
      path_string_ += path_element + "#";
    }
    path_string_ = path_string_.substr(0, path_string_.length() - 1);
  }

  inline entry operator[](const std::string& index) const {
    return entry(db_, path_, index);
  }

  std::string val() {
    std::string ret;
    bool success = db_.get(path_string_, &ret);
    return success ? ret : "";
  }

  bool exists() {
    return db_.check(path_string_);
  }

  bool remove() {
    return db_.remove(path_string_);
  }

  entry& operator=(const std::string& rhs) {
    db_.set(path_string_, rhs);
    return *this;
  };

 private:
  kc::PolyDB& db_;
  std::string path_string_;
  std::vector<std::string> path_;
};

class db {
 public:
  explicit db(const std::string& location)
    : location_(location) {
    if (!db_.open(location, kc::PolyDB::OWRITER | kc::PolyDB::OCREATE)) {
      throw std::exception();
    }
  }

  virtual ~db() {
    if (!db_.close()) {
      std::cerr << "close error: " << db_.error().name() << "\n";
    }
  }

  entry get_entry(const std::string& index) {
    return entry(db_, index);
  }

  kc::PolyDB& get_db_handle() {
    return db_;
  }

 private:
  std::string location_;
  kc::PolyDB db_;
};

}  // namespace botscript_server

#endif  // KC_WRAPPER_
