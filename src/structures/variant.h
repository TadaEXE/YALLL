#pragma once

#include <llvm/IR/Value.h>

namespace yalll {
class ReturnVariant {
 public:
  ReturnVariant();
  ReturnVariant(ReturnVariant&&) = delete;
  ReturnVariant(const ReturnVariant&) = delete;
  ReturnVariant& operator=(ReturnVariant&&) = delete;
  ReturnVariant& operator=(const ReturnVariant&) = delete;
  ~ReturnVariant();

  const llvm::Value* is_error;
  const llvm::Value* return_value;
  const llvm::Value* error_ptr;

};
}  // namespace yalll
