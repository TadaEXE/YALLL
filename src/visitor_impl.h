#pragma once

#include "YALLLBaseVisitor.h"
#include "YALLLParser.h"
#include "ir_generator.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>

namespace yalll
{

class YALLLVisitorImpl : public YALLLBaseVisitor
{
  public:
    YALLLVisitorImpl();

    std::any visitProgram(YALLLParser::ProgramContext* ctx) override;

    std::any visitInterface(YALLLParser::InterfaceContext* ctx) override;

    std::any visitClass(YALLLParser::ClassContext* ctx) override;

    std::any visitEntry_point(YALLLParser::Entry_pointContext* ctx) override;

    // std::any visitStatement(YALLLParser::StatementContext* ctx) override;

    // std::any visitExpression(YALLLParser::ExpressionContext* ctx) override;

    // std::any visitAssignment(YALLLParser::AssignmentContext* ctx) override;

    std::any visitOperation(YALLLParser::OperationContext* ctx) override;

    std::any visitReterr_op(YALLLParser::Reterr_opContext* ctx) override;

  private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;

    // std::unique_ptr<IRGenerator> generator;

    void trigger_function_return();
    void value_is_error();
};
} // namespace yalll
