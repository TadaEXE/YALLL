#include "visitor_impl.h"

#include <llvm/ADT/APInt.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/Constants.h>
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
#include "../value/value.h"
#include "YALLLParser.h"

namespace yallc {

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

inline yalll::Value to_value(llvm::Value* val,
                             typesafety::TypeInformation type_info) {
  return {static_cast<llvm::Value*>(val), type_info};
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

      yalll::Value value = to_value(visit(ctx->op));
      (void)builder->CreateRet(value.llvm_val);

      return std::any();
  }

  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitVar_dec(YALLLParser::Var_decContext* ctx) {
  std::string name = ctx->name->getText();
  size_t type = ctx->ty->getStart()->getType();
  cur_scope->add_field(
      name, yalll::Value(name, typesafety::TypeInformation::from_yalll_t(
                                   type, *context)));
  return std::any();
}

std::any YALLLVisitorImpl::visitVar_def(YALLLParser::Var_defContext* ctx) {
  std::string name = ctx->name->getText();
  yalll::Value value = to_value(visit(ctx->val));
  typesafety::TypeInformation type_info =
      typesafety::TypeInformation::from_yalll_t(ctx->ty->getStart()->getType(),
                                                *context);

  if (type_info.is_compatible(value.type_info)) {
    cur_scope->add_field(name, yalll::Value(name, value.llvm_val, type_info));
  } else {
    incompatible_types(type_info, value.type_info, ctx->getStart()->getLine());
  }

  return std::any();
}

std::any YALLLVisitorImpl::visitIf_else(YALLLParser::If_elseContext* ctx) {

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

  yalll::Value lhs = to_value(visit(ctx->lhs));
  yalll::Value rhs;

  for (auto i = 0; i < ctx->op.size(); ++i) {
    auto op = ctx->op.at(i)->getType();
    rhs = to_value(visit(ctx->rhs.at(i)));

    if (op == YALLLParser::PLUS_SYM) {
      lhs = yalll::Value(builder->CreateAdd(lhs.llvm_val, rhs.llvm_val),
                         lhs.type_info);
    } else if (op == YALLLParser::MINSU_SYM) {
      lhs = yalll::Value(builder->CreateSub(lhs.llvm_val, rhs.llvm_val),
                         lhs.type_info);
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

  yalll::Value lhs = to_value(visit(ctx->lhs));
  yalll::Value rhs;

  for (auto i = 0; i < ctx->op.size(); ++i) {
    auto op = ctx->op.at(i)->getType();
    rhs = to_value(visit(ctx->rhs.at(i)));

    if (!lhs.type_info.is_compatible(rhs.type_info)) {
      incompatible_types(lhs.type_info, rhs.type_info,
                         ctx->rhs.at(i)->getStart()->getLine());
      return lhs;
    }

    if (op == YALLLParser::MUL_SYM) {
      lhs = yalll::Value(builder->CreateMul(lhs.llvm_val, rhs.llvm_val),
                         lhs.type_info);
    } else if (op == YALLLParser::DIV_SYM) {
      if (lhs.type_info.is_signed()) {
        lhs = yalll::Value(builder->CreateSDiv(lhs.llvm_val, rhs.llvm_val),
                           lhs.type_info);
      } else {
        lhs = yalll::Value(builder->CreateUDiv(lhs.llvm_val, rhs.llvm_val),
                           lhs.type_info);
      }
    } else if (op == YALLLParser::MOD_SYM) {
      if (lhs.type_info.is_signed()) {
        lhs = yalll::Value(builder->CreateSRem(lhs.llvm_val, rhs.llvm_val),
                           lhs.type_info);
      } else {
        lhs = yalll::Value(builder->CreateURem(lhs.llvm_val, rhs.llvm_val),
                           lhs.type_info);
      }
    } else {
      std::cout << "Invalid operant in multiplication " << op << std::endl;
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
      return yalll::Value(builder->getInt32(std::stoi(ctx->val->getText())),
                          typesafety::TypeInformation::I32_T(*context));

    case YALLLParser::NAME: {
      auto* value = cur_scope->find_field(ctx->val->getText());
      if (value) {
        return *value;
      } else {
        std::cout << "Undefined variable used. Using i32 0." << std::endl;
        return yalll::Value(builder->getInt32(0),
                            typesafety::TypeInformation::I32_T(*context));
      }
    }

    case YALLLParser::DECIMAL: {
      auto* value = static_cast<llvm::Value*>(llvm::ConstantFP::get(
          *context, llvm::APFloat(std::stof(ctx->val->getText()))));
      return yalll::Value(value, typesafety::TypeInformation::D32_T(*context));
    }

    default:
      std::cout << "Unknown terminal type found " << ctx->val->getText()
                << std::endl;
      break;
  }

  return visitChildren(ctx);
}

}  // namespace yallc
