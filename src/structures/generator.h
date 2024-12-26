#pragma once
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include "../llvmdataholder.h"

namespace gen {

class Generator {
 public:
  explicit Generator(yalll::LLVMDataHolder& holder) : holder(holder) {}
  virtual llvm::Value* generate() const = 0;

 protected:
  yalll::LLVMDataHolder& holder;
};
}  // namespace gen
