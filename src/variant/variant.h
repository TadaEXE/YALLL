#pragma once

#include <llvm/IR/Value.h>

namespace gen {
class ErrorVariant {
 public:
  static llvm::Value* generate(bool is_error, llvm::Value* val);
};
}  // namespace yalll
