#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace yalll
{
class IRGenerator {
public:
  IRGenerator(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& module);
  IRGenerator(IRGenerator &&) = delete;
  IRGenerator(const IRGenerator &) = delete;
  IRGenerator &operator=(IRGenerator &&) = delete;
  IRGenerator &operator=(const IRGenerator &) = delete;
  ~IRGenerator();

  void gen_iserr_check();
  void gen_function_return();

  // Variant load_result();
  // void write_result(Variant& v); 

private:
  llvm::LLVMContext& context;
  llvm::IRBuilder<>& builder;
  llvm::Module& module;
};
}

