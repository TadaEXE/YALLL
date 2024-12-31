#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

namespace yallc {
struct LLVMDataHolder {
public:
  llvm::LLVMContext& context;
  llvm::IRBuilder<>& builder;
  llvm::Module& module;
};
}
