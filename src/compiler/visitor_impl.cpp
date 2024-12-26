#include "visitor_impl.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>

#include <memory>

#include "YALLLParser.h"

namespace yalll {

YALLLVisitorImpl::YALLLVisitorImpl() {
  context = std::make_unique<llvm::LLVMContext>();
  builder = std::make_unique<llvm::IRBuilder<>>(*context);
  module = std::make_unique<llvm::Module>("YALLL", *context);
  // generator = std::make_unique<IRGenerator>(*context, *builder, *module);
}

std::any YALLLVisitorImpl::visitProgram(YALLLParser::ProgramContext* ctx) {
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitInterface(YALLLParser::InterfaceContext* ctx){
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitClass(YALLLParser::ClassContext* ctx){
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitEntry_point(
    YALLLParser::Entry_pointContext* ctx) {
  auto prototype = llvm::FunctionType::get(builder->getInt32Ty(), false);
  auto main_fn = llvm::Function::Create(
      prototype, llvm::Function::ExternalLinkage, "main", module.get());
  auto body = llvm::BasicBlock::Create(*context, "entry", main_fn);
  builder->SetInsertPoint(body);
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitOperation(YALLLParser::OperationContext* ctx) {
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitReterr_op(YALLLParser::Reterr_opContext* ctx) {
  // generator->gen_iserr_check();
  // generator->gen_function_return();
  return visitChildren(ctx);
}
}  // namespace yalll
