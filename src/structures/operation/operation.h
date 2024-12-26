#pragma once

#include <llvm/IR/Value.h>

#include "../generator.h"

namespace gen {

enum class OperationType {
  Addition,
  Substraction,
};

class Operation : Generator {
 public:
  virtual llvm::Value* generate() const override;
};
}  // namespace gen
