#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

namespace yalll {
struct LLVMDataHolder {
public:
  llvm::LLVMContext& context;
  llvm::IRBuilder<>& builder;
  llvm::Module& module;
};
}
