#pragma once

#include <cassert>
namespace yalll {

template <typename T>
class Import {
 public:
  virtual T& get_instance() {
    assert(false && "Unknown type imported!");
    return get_instance();
  }

  T& operator*() { return get_instance(); }

  T* operator->() { return &get_instance(); }
};
}  // namespace yalll
