#pragma once

#include <llvm/IR/Value.h>

#include <vector>

namespace yalll {
class Error {
 public:
  Error();
  Error(Error &&) = delete;
  Error(const Error &) = delete;
  Error &operator=(Error &&) = delete;
  Error &operator=(const Error &) = delete;
  ~Error();

  const llvm::Value *error_type_name;
  const llvm::Value *error_description;
};

class Object {
 public:
  Object();
  Object(Object &&) = delete;
  Object(const Object &) = delete;
  Object &operator=(Object &&) = delete;
  Object &operator=(const Object &) = delete;
  ~Object();

  void build_error_table();

  std::vector<Error> error_table;
  const llvm::Value *object_pool;
};
}  // namespace yalll
