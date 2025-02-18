#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "../logging/logger.h"

#include "../import/import.h"

template <>
llvm::LLVMContext& yalll::Import<llvm::LLVMContext>::get_instance() {
  static llvm::LLVMContext context;
  return context;
}

template <>
llvm::IRBuilder<>& yalll::Import<llvm::IRBuilder<>>::get_instance() {
  yalll::Import<llvm::LLVMContext> context;
  static llvm::IRBuilder<> builder(*context);
  return builder;
}

template <>
util::Logger& yalll::Import<util::Logger>::get_instance() {
  static util::Logger logger;
  return logger;
}
