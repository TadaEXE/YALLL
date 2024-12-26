#pragma once

#include <array>
#include <cstdint>
namespace yalli {

class Stack {
 public:
  template <class T>
  void push(T& obj) {
    static_assert(sizeof(obj) % sizeof(uint64_t) == 0,
                  "Object not aligend to 64bit");

    uint8_t buf[sizeof(T)];
    memcpy(buf, &obj, sizeof(T));

    uint64_t pushe[sizeof(T) / sizeof(uint64_t)] =
        reinterpret_cast<uint64_t*>(buf);
    for (uint64_t i = 0; i < sizeof(T) / sizeof(uint64_t); ++i) {
      stack[sp] = pushe[i];
      ++sp;
    }
  }

  template <class T>
  void pop() {
    sp -= sizeof(T) / sizeof(uint64_t);
  }

  template <class T>
  T* at(uint64_t idx) const {
    std::array<uint64_t, sizeof(T) / sizeof(uint64_t)> buf;
    for (auto i = idx; i < idx + sizeof(T) / sizeof(uint64_t); ++i) {
      buf[i - idx] = stack[i];
    }
    return reinterpret_cast<T*>(buf.data());
  }

 private:
  std::array<uint64_t, 1024 * 100> stack;
  uint64_t sp = 0;
};
}  // namespace yalli
