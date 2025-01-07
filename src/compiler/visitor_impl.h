#pragma once

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <memory>

#include "../scoping/scope.h"
#include "YALLLBaseVisitor.h"
#include "YALLLParser.h"

namespace yallc {

class YALLLVisitorImpl : public YALLLBaseVisitor {
 public:
  YALLLVisitorImpl();
  ~YALLLVisitorImpl();

  std::any visitProgram(YALLLParser::ProgramContext* ctx) override;

  std::any visitInterface(YALLLParser::InterfaceContext* ctx) override;

  std::any visitClass(YALLLParser::ClassContext* ctx) override;

  std::any visitEntry_point(YALLLParser::Entry_pointContext* ctx) override;

  // std::any visitStatement(YALLLParser::StatementContext* ctx) override;

  std::any visitExpression(YALLLParser::ExpressionContext* ctx) override;

  // Declarations
  std::any visitVar_dec(YALLLParser::Var_decContext* ctx) override;

  // Definitions
  std::any visitVar_def(YALLLParser::Var_defContext* ctx) override;

  // If_Else
  std::any visitIf_else(YALLLParser::If_elseContext* ctx) override;
  std::any visitIf(YALLLParser::IfContext* ctx) override;
  std::any visitElse_if(YALLLParser::Else_ifContext* ctx) override;
  std::any visitElse(YALLLParser::ElseContext* ctx) override;

  // Operations
  std::any visitOperation(YALLLParser::OperationContext* ctx) override;
  std::any visitReterr_op(YALLLParser::Reterr_opContext* ctx) override;
  std::any visitAddition_op(YALLLParser::Addition_opContext* ctx) override;
  std::any visitMultiplication_op(
      YALLLParser::Multiplication_opContext* ctx) override;
  // ==Primary_op==============================================================
  std::any visitPrimary_op_high_precedence(
      YALLLParser::Primary_op_high_precedenceContext* ctx) override;
  std::any visitPrimary_op_fc(YALLLParser::Primary_op_fcContext* ctx) override;
  std::any visitPrimary_op_term(
      YALLLParser::Primary_op_termContext* ctx) override;
  // ==========================================================================
  std::any visitTerminal_op(YALLLParser::Terminal_opContext* ctx) override;

 private:
  std::unique_ptr<llvm::LLVMContext> context;
  std::unique_ptr<llvm::IRBuilder<>> builder;
  std::unique_ptr<llvm::Module> module;

  // std::unique_ptr<IRGenerator> generator;

  void trigger_function_return();
  void value_is_error();

  std::unique_ptr<scoping::Scope> base_scope;
  scoping::Scope* cur_scope;
};
}  // namespace yallc
