#pragma once

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <memory>

#include "../import/import.h"
#include "../scoping/scope.h"
#include "YALLLBaseVisitor.h"
#include "YALLLParser.h"

namespace yallc {

class YALLLVisitorImpl : public YALLLBaseVisitor {
 public:
  YALLLVisitorImpl(std::string out_path);
  ~YALLLVisitorImpl();

  std::any visitProgram(YALLLParser::ProgramContext* ctx) override;
  std::any visitInterface(YALLLParser::InterfaceContext* ctx) override;
  std::any visitClass(YALLLParser::ClassContext* ctx) override;
  std::any visitEntry_point(YALLLParser::Entry_pointContext* ctx) override;
  // std::any visitStatement(YALLLParser::StatementContext* ctx) override;
  std::any visitExpression(YALLLParser::ExpressionContext* ctx) override;
  std::any visitBlock(YALLLParser::BlockContext* ctx) override;
  std::any visitAssignment(YALLLParser::AssignmentContext* ctx) override;
  std::any visitParameter_list(YALLLParser::Parameter_listContext* ctx) override;

  // Declarations
  std::any visitVar_dec(YALLLParser::Var_decContext* ctx) override;

  // Definitions
  std::any visitVar_def(YALLLParser::Var_defContext* ctx) override;
  std::any visitFunction_def(YALLLParser::Function_defContext* ctx) override;

  // If_Else
  std::any visitIf_else(YALLLParser::If_elseContext* ctx) override;
  std::any visitIf(YALLLParser::IfContext* ctx) override;
  std::any visitElse_if(YALLLParser::Else_ifContext* ctx) override;
  std::any visitElse(YALLLParser::ElseContext* ctx) override;

  // Operations
  std::any visitOperation(YALLLParser::OperationContext* ctx) override;
  std::any visitReterr_op(YALLLParser::Reterr_opContext* ctx) override;
  std::any visitBool_or_op(YALLLParser::Bool_or_opContext* ctx) override;
  std::any visitBool_and_op(YALLLParser::Bool_and_opContext* ctx) override;
  std::any visitCompare_op(YALLLParser::Compare_opContext* ctx) override;
  std::any visitAddition_op(YALLLParser::Addition_opContext* ctx) override;
  std::any visitMultiplication_op(
      YALLLParser::Multiplication_opContext* ctx) override;
  // std::any visitUnary_op(YALLLParser::Unary_opContext* ctx) override;
  // ==Primary_op==============================================================
  std::any visitPrimary_op_high_precedence(
      YALLLParser::Primary_op_high_precedenceContext* ctx) override;
  std::any visitPrimary_op_fc(YALLLParser::Primary_op_fcContext* ctx) override;
  std::any visitPrimary_op_term(
      YALLLParser::Primary_op_termContext* ctx) override;
  // ==========================================================================
  std::any visitTerminal_op(YALLLParser::Terminal_opContext* ctx) override;

 private:
  yalll::Import<llvm::LLVMContext> context;
  yalll::Import<llvm::IRBuilder<>> builder;
  std::unique_ptr<llvm::Module> module;

  void trigger_function_return();
  void value_is_error();

  scoping::Scope cur_scope;

  std::string out_path;
};
}  // namespace yallc
