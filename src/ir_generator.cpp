#include "ir_generator.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/MathExtras.h>

namespace yalll
{
IRGenerator::IRGenerator(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Module& module) : context(context), builder(builder), module(module) {}
}

