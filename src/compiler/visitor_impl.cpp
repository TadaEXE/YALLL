#include "visitor_impl.h"

#include <llvm/ADT/APInt.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include <any>
#include <iostream>
#include <memory>
#include <ostream>
#include <source_location>
#include <string>

#include "../scoping/scope.h"
#include "YALLLParser.h"

namespace yallc {

inline llvm::Value* to_value(
    std::any any,
    std::source_location location = std::source_location::current()) {
  try {
    return std::any_cast<llvm::Value*>(any);
  } catch (const std::bad_any_cast& cast) {
    std::cout << location.file_name() << ":" << std::endl
              << location.function_name() << "@" << location.line()
              << ": Faild to cast " << any.type().name() << " to llvm::Value* "
              << cast.what() << std::endl;
    throw cast;
  }
}

inline llvm::Value* to_value(llvm::Value* val) {
  return static_cast<llvm::Value*>(val);
}

YALLLVisitorImpl::YALLLVisitorImpl() {
  context = std::make_unique<llvm::LLVMContext>();
  builder = std::make_unique<llvm::IRBuilder<>>(*context);
  module = std::make_unique<llvm::Module>("YALLL", *context);

  base_scope = std::make_unique<scoping::Scope>();
  cur_scope = base_scope->push();
}

YALLLVisitorImpl::~YALLLVisitorImpl() {}

std::any YALLLVisitorImpl::visitProgram(YALLLParser::ProgramContext* ctx) {
  auto res = visitChildren(ctx);

  std::error_code ec;
  llvm::raw_fd_ostream llvm_out("../../output.ll", ec);
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

  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitExpression(
    YALLLParser::ExpressionContext* ctx) {
  switch (ctx->getStart()->getType()) {
    case YALLLParser::RETURN_KW:
      if (ctx->children.size() == 2) {
        return to_value(builder->CreateRetVoid());
      }
      return to_value(builder->CreateRet(to_value(visit(ctx->children.at(1)))));
  }

  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitVar_dec(YALLLParser::Var_decContext* ctx) {
  cur_scope->add_field(ctx->children.at(1)->getText(), nullptr);
  return std::any();
}

std::any YALLLVisitorImpl::visitVar_def(YALLLParser::Var_defContext* ctx) {
  auto value = to_value(visit(ctx->children.at(3)));
  cur_scope->add_field(ctx->children.at(1)->getText(), value);
  return std::any();
}

std::any YALLLVisitorImpl::visitOperation(YALLLParser::OperationContext* ctx) {
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitReterr_op(YALLLParser::Reterr_opContext* ctx) {
  // visit(ctx->iserr_op());
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitAddition_op(
    YALLLParser::Addition_opContext* ctx) {
  if (ctx->children.size() == 1) return visitChildren(ctx);

  llvm::Value* lhs = to_value(visit(ctx->lhs));
  llvm::Value* rhs;

  for (auto i = 0; i < ctx->op.size(); ++i) {
    auto op = ctx->op.at(i)->getType();
    rhs = to_value(visit(ctx->rhs.at(i)));

    if (op == YALLLParser::PLUS_SYM) {
      lhs = to_value(builder->CreateAdd(lhs, rhs));
    } else if (op == YALLLParser::MINSU_SYM) {
      lhs = to_value(builder->CreateSub(lhs, rhs));
    } else {
      std::cout << "Invalid operant in addition " << op << std::endl;
      break;
    }
  }

  return lhs;
}

std::any YALLLVisitorImpl::visitMultiplication_op(
    YALLLParser::Multiplication_opContext* ctx) {
  if (ctx->children.size() == 1) return visitChildren(ctx);

  llvm::Value* lhs = to_value(visit(ctx->lhs));
  llvm::Value* rhs;

  for (auto i = 0; i < ctx->op.size(); ++i) {
    auto op = ctx->op.at(i)->getType();
    rhs = to_value(visit(ctx->rhs.at(i)));

    if (op == YALLLParser::MUL_SYM) {
      lhs = to_value(builder->CreateMul(lhs, rhs));
    } else if (op == YALLLParser::DIV_SYM) {
      lhs = to_value(builder->CreateSDiv(lhs, rhs));
    } else if (op == YALLLParser::MOD_SYM) {
      lhs = to_value(builder->CreateSRem(lhs, rhs));
    } else {
      std::cout << "Invalid operant in addition " << op << std::endl;
      break;
    }
  }

  return lhs;
}

std::any YALLLVisitorImpl::visitPrimary_op_high_precedence(
    YALLLParser::Primary_op_high_precedenceContext* ctx) {
  return to_value(visit(ctx->val));
}

std::any YALLLVisitorImpl::visitPrimary_op_fc(
    YALLLParser::Primary_op_fcContext* ctx) {
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitPrimary_op_term(
    YALLLParser::Primary_op_termContext* ctx) {
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitTerminal_op(
    YALLLParser::Terminal_opContext* ctx) {
  switch (ctx->val->getType()) {
    case YALLLParser::INTEGER:
      return to_value(builder->getInt32(std::stoi(ctx->val->getText())));

    case YALLLParser::NAME:
      return to_value(cur_scope->find_field(ctx->val->getText()));

    default:
      std::cout << "Unknown terminal type found " << ctx->val->getText()
                << std::endl;
      break;
  }

  return visitChildren(ctx);
}

}  // namespace yallc
