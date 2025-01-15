#include "visitor_impl.h"

#include <llvm/ADT/APInt.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>
#include <tree/ParseTree.h>
#include <tree/ParseTreeType.h>
#include <tree/ParseTreeWalker.h>

#include <any>
#include <iostream>
#include <memory>
#include <ostream>
#include <source_location>
#include <string>
#include <vector>

#include "../operation/operation.h"
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

inline yalll::Operation to_operation(
    std::any any,
    std::source_location location = std::source_location::current()) {
  try {
    return std::any_cast<yalll::Operation>(any);
  } catch (const std::bad_any_cast& cast) {
    std::cout << location.file_name() << ":" << std::endl
              << location.function_name() << "a" << location.line()
              << ": Failed to cast " << any.type().name()
              << " to yalll::Operation " << cast.what() << std::endl;
    throw cast;
  }
}

YALLLVisitorImpl::YALLLVisitorImpl(std::string out_path) : out_path(out_path) {
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

  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitExpression(
    YALLLParser::ExpressionContext* ctx) {
  switch (ctx->getStart()->getType()) {
    case YALLLParser::RETURN_KW:
      if (ctx->children.size() == 2) {
        (void)builder->CreateRetVoid();
        return std::any();
      }

      auto operation = to_operation(visit(ctx->ret_val));
      // if (operation.resolve_with_type_info(cur_scope->get_ret_type_info()))
      if (operation.resolve_without_type_info())
        (void)builder->CreateRet(operation.generate_llvm_val());

      return std::any();
  }

  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitAssignment(
    YALLLParser::AssignmentContext* ctx) {
  auto* variable = cur_scope->find_field(ctx->name->getText());

  if (variable) {
    auto operation = to_operation(visit(ctx->val));
    if (operation.resolve_with_type_info(variable->type_info)) {
      variable->llvm_val = operation.generate_llvm_val();
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
  cur_scope->add_field(
      name,
      yalll::Value(typesafety::TypeInformation::from_yalll_t(type, *context),
                   nullptr, *builder, ctx->name->getLine(), name));
  return std::any();
}

std::any YALLLVisitorImpl::visitVar_def(YALLLParser::Var_defContext* ctx) {
  std::string name = ctx->name->getText();
  yalll::Operation operation = to_operation(visit(ctx->val));
  typesafety::TypeInformation type_info =
      typesafety::TypeInformation::from_yalll_t(ctx->ty->getStart()->getType(),
                                                *context);

  if (operation.resolve_with_type_info(type_info)) {
    cur_scope->add_field(
        name, yalll::Value(type_info, operation.generate_llvm_val(), *builder,
                           ctx->getStart()->getLine(), name));
  }
  return std::any();
}

std::any YALLLVisitorImpl::visitIf_else(YALLLParser::If_elseContext* ctx) {
  // cur_scope = cur_scope->push();
  // auto if_cmp = to_value(visit(ctx->if_br->cmp));
  // builder->Create
  // cur_scope = cur_scope->pop();
  //
  // for (auto* else_if_br : ctx->else_if_brs) {
  //   cur_scope = cur_scope->push();
  //   visit(else_if_br);
  //   cur_scope = cur_scope->pop();
  // }
  //
  // if (ctx->else_br) {
  //   cur_scope = cur_scope->push();
  //   visit(ctx->else_br);
  //   cur_scope = cur_scope->pop();
  // }
  std::cout << "Not implemented yet" << std::endl;
  return std::any();
}

std::any YALLLVisitorImpl::visitIf(YALLLParser::IfContext* ctx) {
  std::cout << "Compiler error" << std::endl;
  return std::any();
}

std::any YALLLVisitorImpl::visitElse_if(YALLLParser::Else_ifContext* ctx) {
  std::cout << "Compiler error" << std::endl;
  return std::any();
}

std::any YALLLVisitorImpl::visitElse(YALLLParser::ElseContext* ctx) {
  std::cout << "Compiler error" << std::endl;
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

  std::vector<yalll::Operation> operations;
  std::vector<size_t> op_codes;

  operations.push_back(to_operation(visit(ctx->lhs)));

  for (auto* rhs : ctx->rhs) {
    operations.push_back(to_operation(visit(rhs)));
    op_codes.push_back(ctx->op->getType());
  }

  return yalll::Operation(operations, op_codes);
}

std::any YALLLVisitorImpl::visitBool_and_op(
    YALLLParser::Bool_and_opContext* ctx) {
  if (ctx->rhs.size() == 0) return visit(ctx->lhs);

  std::vector<yalll::Operation> operations;
  std::vector<size_t> op_codes;

  operations.push_back(to_operation(visit(ctx->lhs)));

  for (auto* rhs : ctx->rhs) {
    operations.push_back(to_operation(visit(rhs)));
    op_codes.push_back(ctx->op->getType());
  }

  return yalll::Operation(operations, op_codes);
}

std::any YALLLVisitorImpl::visitCompare_op(
    YALLLParser::Compare_opContext* ctx) {
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitAddition_op(
    YALLLParser::Addition_opContext* ctx) {
  if (ctx->rhs.size() == 0) return visit(ctx->lhs);

  std::vector<yalll::Operation> operations;
  std::vector<size_t> op_codes;

  operations.push_back(to_operation(visit(ctx->lhs)));

  for (auto i = 0; i < ctx->op.size(); ++i) {
    operations.push_back(to_operation(visit(ctx->rhs.at(i))));
    op_codes.push_back(ctx->op.at(i)->getType());
  }

  return yalll::Operation(operations, op_codes);
}

std::any YALLLVisitorImpl::visitMultiplication_op(
    YALLLParser::Multiplication_opContext* ctx) {
  if (ctx->rhs.size() == 0) return visit(ctx->lhs);

  std::vector<yalll::Operation> operations;
  std::vector<size_t> op_codes;

  operations.push_back(to_operation(visit(ctx->lhs)));

  for (auto i = 0; i < ctx->op.size(); ++i) {
    operations.push_back(to_operation(visit(ctx->rhs.at(i))));
    op_codes.push_back(ctx->op.at(i)->getType());
  }

  return yalll::Operation(operations, op_codes);
}

std::any YALLLVisitorImpl::visitPrimary_op_high_precedence(
    YALLLParser::Primary_op_high_precedenceContext* ctx) {
  auto tmp = to_operation(visit(ctx->val));
  return tmp;
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
      return yalll::Operation(
          yalll::Value(typesafety::TypeInformation::INTAUTO_T(*context),
                       ctx->val->getText(), *builder, ctx->val->getLine()));

    case YALLLParser::NAME: {
      auto* value = cur_scope->find_field(ctx->val->getText());
      if (value) {
        return *value;
      } else {
        std::cout << "Undefined variable used " << ctx->val->getText()
                  << std::endl;
        return yalll::Operation(
            yalll::Value(typesafety::TypeInformation::VOID_T(*context),
                         llvm::PoisonValue::get(builder->getVoidTy()), *builder,
                         ctx->val->getLine()));
      }
    }

    case YALLLParser::DECIMAL:
      return yalll::Operation(
          yalll::Value(typesafety::TypeInformation::DECAUTO_T(*context),
                       ctx->val->getText(), *builder, ctx->val->getLine()));

    case YALLLParser::BOOL_TRUE:
      return yalll::Operation(
          yalll::Value(typesafety::TypeInformation::BOOL_T(*context),
                       builder->getInt1(true), *builder, ctx->val->getLine()));

    case YALLLParser::BOOL_FALSE:
      return yalll::Operation(
          yalll::Value(typesafety::TypeInformation::BOOL_T(*context),
                       builder->getInt1(false), *builder, ctx->val->getLine()));

    case YALLLParser::NULL_VALUE:
      return yalll::Operation(
          yalll::Value::NULL_VALUE(*builder, ctx->val->getLine()));

    default:
      std::cout << "Unknown terminal type found " << ctx->val->getText()
                << std::endl;
      break;
  }

  return visitChildren(ctx);
}

}  // namespace yallc
