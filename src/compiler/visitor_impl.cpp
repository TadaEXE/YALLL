#include "visitor_impl.h"

#include <llvm/ADT/APInt.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetOptions.h>
#include <tree/ParseTree.h>
#include <tree/ParseTreeType.h>
#include <tree/ParseTreeWalker.h>

#include <algorithm>
#include <any>
#include <iostream>
#include <memory>
#include <ostream>
#include <source_location>
#include <string>
#include <typeinfo>
#include <vector>

#include "../operation/addoperation.h"
#include "../operation/andoperation.h"
#include "../operation/cmpoperation.h"
#include "../operation/muloperation.h"
#include "../operation/operation.h"
#include "../operation/oroperation.h"
#include "../operation/terminaloperation.h"
#include "../scoping/scope.h"
#include "../value/value.h"
#include "YALLLParser.h"

namespace yallc {

// std::string get_assigned_name(YALLLParser::Terminal_opContext* ctx) {
//   antlr4::tree::ParseTree* parent = ctx->parent;
//
//   while (parent->parent) {
//     if (auto assigment =
//             dynamic_cast<YALLLParser::AssignmentContext*>(parent)) {
//       return assigment->name->getText();
//     }
//     if (auto var_def = dynamic_cast<YALLLParser::Var_defContext*>(parent)) {
//       return var_def->name->getText();
//     }
//     parent = parent->parent;
//   }
//   return "";
// }

inline void incompatible_types(typesafety::TypeInformation& lhs,
                               typesafety::TypeInformation& rhs, size_t line) {
  std::cout << "Incompatible types " << lhs.to_string() << " and "
            << rhs.to_string() << " in line " << line << std::endl;
}

inline yalll::Value to_value(
    std::any any,
    std::source_location location = std::source_location::current()) {
  try {
    return std::any_cast<yalll::Value>(any);
  } catch (const std::bad_any_cast& cast) {
    std::cout << location.file_name() << ":" << std::endl
              << location.function_name() << "@" << location.line()
              << ": Faild to cast " << any.type().name() << " to yalll::Value "
              << cast.what() << std::endl;
    throw cast;
  }
}

inline std::shared_ptr<yalll::Operation> to_operation(
    std::any any,
    std::source_location location = std::source_location::current()) {
  try {
    auto type = any.type().hash_code();
    if (type == typeid(std::shared_ptr<yalll::AddOperation>).hash_code())
      return std::any_cast<std::shared_ptr<yalll::AddOperation>>(any);
    if (type == typeid(std::shared_ptr<yalll::MulOperation>).hash_code())
      return std::any_cast<std::shared_ptr<yalll::MulOperation>>(any);
    if (type == typeid(std::shared_ptr<yalll::AndOperation>).hash_code())
      return std::any_cast<std::shared_ptr<yalll::AndOperation>>(any);
    if (type == typeid(std::shared_ptr<yalll::OrOperation>).hash_code())
      return std::any_cast<std::shared_ptr<yalll::OrOperation>>(any);
    if (type == typeid(std::shared_ptr<yalll::CmpOperation>).hash_code())
      return std::any_cast<std::shared_ptr<yalll::CmpOperation>>(any);
    if (type == typeid(std::shared_ptr<yalll::TerminalOperation>).hash_code())
      return std::any_cast<std::shared_ptr<yalll::TerminalOperation>>(any);

    return std::any_cast<std::shared_ptr<yalll::Operation>>(any);
  } catch (const std::bad_any_cast& cast) {
    std::cout << location.file_name() << ":" << std::endl
              << location.function_name() << "@" << location.line() << std::endl
              << ": Failed to cast " << any.type().name()
              << " to std::shared_ptr<yalll::Operation> " << cast.what()
              << std::endl;
    throw cast;
  }
}

YALLLVisitorImpl::YALLLVisitorImpl(std::string out_path) : out_path(out_path) {
  context = std::make_unique<llvm::LLVMContext>();
  builder = std::make_unique<llvm::IRBuilder<>>(*context);
  module = std::make_unique<llvm::Module>("YALLL", *context);
}

YALLLVisitorImpl::~YALLLVisitorImpl() {}

std::any YALLLVisitorImpl::visitProgram(YALLLParser::ProgramContext* ctx) {
  auto res = visitChildren(ctx);

  std::error_code ec;
  llvm::raw_fd_ostream llvm_out(out_path, ec);
  module->print(llvm_out, nullptr);

  return res;
}

std::any YALLLVisitorImpl::visitInterface(YALLLParser::InterfaceContext* ctx) {
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitClass(YALLLParser::ClassContext* ctx) {
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitEntry_point(
    YALLLParser::Entry_pointContext* ctx) {
  auto prototype = llvm::FunctionType::get(builder->getInt32Ty(), false);
  auto main_fn = llvm::Function::Create(
      prototype, llvm::Function::ExternalLinkage, "main", module.get());
  auto body = llvm::BasicBlock::Create(*context, "entry", main_fn);
  builder->SetInsertPoint(body);

  visitChildren(ctx);

  // ensure error exit if no return given by program
  builder->CreateRet(llvm::ConstantInt::getSigned(builder->getInt32Ty(), -1));
  return std::any();
}

// std::any YALLLVisitorImpl::visitStatement(YALLLParser::StatementContext* ctx)
// {
//
// }

std::any YALLLVisitorImpl::visitExpression(
    YALLLParser::ExpressionContext* ctx) {
  switch (ctx->getStart()->getType()) {
    case YALLLParser::RETURN_KW:
      if (ctx->children.size() == 2) {
        (void)builder->CreateRetVoid();
        return std::any();
      }
      std::cout << ctx->getText() << std::endl;
      auto operation = to_operation(visit(ctx->ret_val));
      // if (operation.resolve_with_type_info(cur_scope.get_ret_type_info()))
      if (operation->resolve_without_type_info(*context))
        (void)builder->CreateRet(
            operation->generate_value(*builder).get_llvm_val());

      return std::any();
  }

  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitBlock(YALLLParser::BlockContext* ctx) {
  cur_scope.push();
  for (auto* statement : ctx->statements) {
    std::cout << statement->getText() << std::endl;
    visit(statement);
  }
  cur_scope.pop();
  return std::any();
}

std::any YALLLVisitorImpl::visitAssignment(
    YALLLParser::AssignmentContext* ctx) {
  auto* variable = cur_scope.find_field(ctx->name->getText());

  if (variable) {
    auto operation = to_operation(visit(ctx->val));
    if (operation->resolve_with_type_info(variable->type_info, *context)) {
      variable->llvm_val = operation->generate_value(*builder).get_llvm_val();
    }
  } else {
    std::cout << "Undeclared variable " << ctx->name->getText()
              << " used in line " << ctx->name->getLine() << std::endl;
  }

  return std::any();
}

std::any YALLLVisitorImpl::visitVar_dec(YALLLParser::Var_decContext* ctx) {
  std::string name = ctx->name->getText();
  size_t type = ctx->ty->getStart()->getType();
  cur_scope.add_field(
      name,
      yalll::Value(typesafety::TypeInformation::from_yalll_t(type, *context),
                   nullptr, *builder, ctx->name->getLine(), name));
  return std::any();
}

std::any YALLLVisitorImpl::visitVar_def(YALLLParser::Var_defContext* ctx) {
  std::string name = ctx->name->getText();
  auto operation = to_operation(visit(ctx->val));
  typesafety::TypeInformation type_info =
      typesafety::TypeInformation::from_yalll_t(ctx->ty->getStart()->getType(),
                                                *context);

  if (operation->resolve_with_type_info(type_info, *context)) {
    cur_scope.add_field(
        name, yalll::Value(type_info,
                           operation->generate_value(*builder).get_llvm_val(),
                           *builder, ctx->getStart()->getLine(), name));
  }
  return std::any();
}

std::any YALLLVisitorImpl::visitFunction_def(YALLLParser::Function_defContext* ctx) {
  std::string name = ctx->func_name->getText();

}

std::any YALLLVisitorImpl::visitIf_else(YALLLParser::If_elseContext* ctx) {
  auto if_true = llvm::BasicBlock::Create(*context, "if_true",
                                          module->getFunction("main"));
  auto if_false = llvm::BasicBlock::Create(*context, "if_false",
                                           module->getFunction("main"));
  auto if_exit = llvm::BasicBlock::Create(*context, "if_exit",
                                          module->getFunction("main"));

  auto if_cmp = to_operation(visit(ctx->if_br->cmp));
  if (if_cmp->resolve_with_type_info(
          typesafety::TypeInformation::BOOL_T(*context), *context)) {
    auto cmp_value = if_cmp->generate_value(*builder);
    builder->CreateCondBr(cmp_value.get_llvm_val(), if_true, if_false);

    builder->SetInsertPoint(if_true);
    visit(ctx->if_br->body);
    builder->CreateBr(if_exit);

    builder->SetInsertPoint(if_false);
    for (auto* else_if_br : ctx->else_if_brs) {
      std::cout << else_if_br->cmp->getText() << std::endl;

      auto else_if_true = llvm::BasicBlock::Create(*context, "else_if_true",
                                                   module->getFunction("main"));
      auto else_if_false = llvm::BasicBlock::Create(
          *context, "else_if_false", module->getFunction("main"));

      auto else_if_cmp = to_operation(visit(else_if_br->cmp));
      if (else_if_cmp->resolve_with_type_info(
              typesafety::TypeInformation::BOOL_T(*context), *context)) {
        auto else_if_cmp_value = else_if_cmp->generate_value(*builder);
        builder->CreateCondBr(else_if_cmp_value.get_llvm_val(), else_if_true,
                              else_if_false);

        builder->SetInsertPoint(else_if_true);
        visit(else_if_br->body);
        builder->CreateBr(if_exit);
        builder->SetInsertPoint(else_if_false);
      }
    }

    auto else_case = llvm::BasicBlock::Create(*context, "else_case",
                                              module->getFunction("main"));
    if (ctx->else_br) {
      builder->CreateBr(else_case);

      builder->SetInsertPoint(else_case);
      visit(ctx->else_br->body);
    }

    if_exit->moveAfter(else_case);
    builder->CreateBr(if_exit);
    builder->SetInsertPoint(if_exit);
  }
  return std::any();
}

std::any YALLLVisitorImpl::visitIf(YALLLParser::IfContext* ctx) {
  std::cout << "Compiler error if" << std::endl;
  return std::any();
}

std::any YALLLVisitorImpl::visitElse_if(YALLLParser::Else_ifContext* ctx) {
  std::cout << "Compiler error else if" << std::endl;
  return std::any();
}

std::any YALLLVisitorImpl::visitElse(YALLLParser::ElseContext* ctx) {
  std::cout << "Compiler error else" << std::endl;
  return std::any();
}

std::any YALLLVisitorImpl::visitOperation(YALLLParser::OperationContext* ctx) {
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitReterr_op(YALLLParser::Reterr_opContext* ctx) {
  // visit(ctx->iserr_op());
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitBool_or_op(
    YALLLParser::Bool_or_opContext* ctx) {
  if (ctx->rhs.size() == 0) return visit(ctx->lhs);

  std::vector<std::shared_ptr<yalll::Operation>> operations;
  std::vector<size_t> op_codes;

  operations.push_back(to_operation(visit(ctx->lhs)));

  for (auto* rhs : ctx->rhs) {
    operations.push_back(to_operation(visit(rhs)));
    op_codes.push_back(ctx->op->getType());
  }

  return std::make_shared<yalll::OrOperation>(operations, op_codes);
}

std::any YALLLVisitorImpl::visitBool_and_op(
    YALLLParser::Bool_and_opContext* ctx) {
  if (ctx->rhs.size() == 0) return visit(ctx->lhs);

  std::vector<std::shared_ptr<yalll::Operation>> operations;
  std::vector<size_t> op_codes;

  operations.push_back(to_operation(visit(ctx->lhs)));

  for (auto* rhs : ctx->rhs) {
    operations.push_back(to_operation(visit(rhs)));
    op_codes.push_back(ctx->op->getType());
  }

  return std::make_shared<yalll::AndOperation>(operations, op_codes);
}

std::any YALLLVisitorImpl::visitCompare_op(
    YALLLParser::Compare_opContext* ctx) {
  if (ctx->rhs.size() == 0) return visit(ctx->lhs);

  std::vector<std::shared_ptr<yalll::Operation>> operations;
  std::vector<size_t> op_codes;

  operations.push_back(to_operation(visit(ctx->lhs)));

  for (auto i = 0; i < ctx->op.size(); ++i) {
    operations.push_back(to_operation(visit(ctx->rhs.at(i))));
    op_codes.push_back(ctx->op.at(i)->getStart()->getType());
  }

  return std::make_shared<yalll::CmpOperation>(operations, op_codes);
}

std::any YALLLVisitorImpl::visitAddition_op(
    YALLLParser::Addition_opContext* ctx) {
  if (ctx->rhs.size() == 0) return visit(ctx->lhs);

  std::vector<std::shared_ptr<yalll::Operation>> operations;
  std::vector<size_t> op_codes;

  operations.push_back(to_operation(visit(ctx->lhs)));

  for (auto i = 0; i < ctx->op.size(); ++i) {
    operations.push_back(to_operation(visit(ctx->rhs.at(i))));
    op_codes.push_back(ctx->op.at(i)->getType());
  }

  return std::make_shared<yalll::AddOperation>(operations, op_codes);
}

std::any YALLLVisitorImpl::visitMultiplication_op(
    YALLLParser::Multiplication_opContext* ctx) {
  if (ctx->rhs.size() == 0) return visit(ctx->lhs);

  std::vector<std::shared_ptr<yalll::Operation>> operations;
  std::vector<size_t> op_codes;

  operations.push_back(to_operation(visit(ctx->lhs)));

  for (auto i = 0; i < ctx->op.size(); ++i) {
    operations.push_back(to_operation(visit(ctx->rhs.at(i))));
    op_codes.push_back(ctx->op.at(i)->getType());
  }

  return std::make_shared<yalll::MulOperation>(operations, op_codes);
}

std::any YALLLVisitorImpl::visitPrimary_op_high_precedence(
    YALLLParser::Primary_op_high_precedenceContext* ctx) {
  auto tmp = to_operation(visit(ctx->val));
  return std::make_shared<yalll::Operation>(
      std::vector<std::shared_ptr<yalll::Operation>>{tmp},
      std::vector<size_t>{});
}

std::any YALLLVisitorImpl::visitPrimary_op_fc(
    YALLLParser::Primary_op_fcContext* ctx) {
  return to_operation(visit(ctx->val));
}

std::any YALLLVisitorImpl::visitPrimary_op_term(
    YALLLParser::Primary_op_termContext* ctx) {
  return to_operation(visit(ctx->val));
}

std::any YALLLVisitorImpl::visitTerminal_op(
    YALLLParser::Terminal_opContext* ctx) {
  std::cout << ctx->getText() << std::endl;
  switch (ctx->val->getType()) {
    case YALLLParser::INTEGER:
      return std::make_shared<yalll::TerminalOperation>(
          yalll::Value(typesafety::TypeInformation::INTAUTO_T(*context),
                       ctx->val->getText(), *builder, ctx->val->getLine()));

    case YALLLParser::NAME: {
      auto* value = cur_scope.find_field(ctx->val->getText());
      std::cout << "Visiting terminal" << std::endl;
      if (value) {
        return std::make_shared<yalll::TerminalOperation>(*value);
      } else {
        std::cout << "Undefined variable used " << ctx->val->getText()
                  << std::endl;
        return std::make_shared<yalll::TerminalOperation>(
            yalll::Value(typesafety::TypeInformation::VOID_T(*context),
                         llvm::PoisonValue::get(builder->getVoidTy()), *builder,
                         ctx->val->getLine()));
      }
    }

    case YALLLParser::DECIMAL:
      return std::make_shared<yalll::TerminalOperation>(
          yalll::Value(typesafety::TypeInformation::DECAUTO_T(*context),
                       ctx->val->getText(), *builder, ctx->val->getLine()));

    case YALLLParser::BOOL_TRUE:
      return std::make_shared<yalll::TerminalOperation>(
          yalll::Value(typesafety::TypeInformation::BOOL_T(*context),
                       builder->getInt1(true), *builder, ctx->val->getLine()));

    case YALLLParser::BOOL_FALSE:
      return std::make_shared<yalll::TerminalOperation>(
          yalll::Value(typesafety::TypeInformation::BOOL_T(*context),
                       builder->getInt1(false), *builder, ctx->val->getLine()));

    case YALLLParser::NULL_VALUE:
      return std::make_shared<yalll::TerminalOperation>(
          yalll::Value::NULL_VALUE(*builder, ctx->val->getLine()));

    default:
      std::cout << "Unknown terminal type found " << ctx->val->getText()
                << std::endl;
      return std::any();
      break;
  }
}

}  // namespace yallc
