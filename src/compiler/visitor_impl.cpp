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

// inline yalll::Value to_value(llvm::Value* val,
//                              typesafety::TypeInformation type_info) {
//   return {type_info, static_cast<llvm::Value*>(val), };
// }

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
      (void)builder->CreateRet(value.get_llvm_val(*builder));

      return std::any();
  }

  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitAssignment(
    YALLLParser::AssignmentContext* ctx) {
  auto* variable = cur_scope->find_field(ctx->name->getText());

  if (variable) {
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
                   nullptr, ctx->name->getLine(), name));
  return std::any();
}

std::any YALLLVisitorImpl::visitVar_def(YALLLParser::Var_defContext* ctx) {
  std::string name = ctx->name->getText();
  yalll::Value value = to_value(visit(ctx->val));
  typesafety::TypeInformation type_info =
      typesafety::TypeInformation::from_yalll_t(ctx->ty->getStart()->getType(),
                                                *context);

  if (type_info.make_compatible(value.type_info)) {
    cur_scope->add_field(
        name, yalll::Value(type_info, value.get_llvm_val(*builder), ctx->getStart()->getLine(), name));
  } else {
    incompatible_types(type_info, value.type_info, ctx->getStart()->getLine());
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

  yalll::Value lhs = to_value(visit(ctx->lhs));
  auto bool_t = typesafety::TypeInformation::BOOL_T(*context);
  if (!lhs.type_info.make_compatible(bool_t)) {
    std::cout << "Non bool type used in boolean or operation" << std::endl;
    incompatible_types(lhs.type_info, bool_t, ctx->lhs->getStart()->getLine());
  }
  yalll::Value rhs;

  for (auto i = 0; i < ctx->rhs.size(); ++i) {
    rhs = to_value(visit(ctx->rhs.at(i)));
    if (!rhs.type_info.make_compatible(bool_t)) {
      std::cout << "Non bool type used in boolean or operation" << std::endl;
      incompatible_types(rhs.type_info, bool_t,
                         ctx->rhs.at(i)->getStart()->getLine());
    }
    lhs = yalll::Value(lhs.type_info,
                       builder->CreateOr(lhs.get_llvm_val(*builder),
                                         rhs.get_llvm_val(*builder)), ctx->rhs.at(i)->getStart()->getLine());
  }

  return lhs;
}

std::any YALLLVisitorImpl::visitBool_and_op(
    YALLLParser::Bool_and_opContext* ctx) {
  if (ctx->rhs.size() == 0) return visit(ctx->lhs);

  yalll::Value lhs = to_value(visit(ctx->lhs));
  auto bool_t = typesafety::TypeInformation::BOOL_T(*context);
  if (!lhs.type_info.make_compatible(bool_t)) {
    std::cout << "Non bool type used in boolean and operation" << std::endl;
    incompatible_types(lhs.type_info, bool_t, ctx->lhs->getStart()->getLine());
  }
  yalll::Value rhs;

  for (auto i = 0; i < ctx->rhs.size(); ++i) {
    rhs = to_value(visit(ctx->rhs.at(i)));
    if (!rhs.type_info.make_compatible(bool_t)) {
      std::cout << "Non bool type used in boolean or operation" << std::endl;
      incompatible_types(rhs.type_info, bool_t,
                         ctx->rhs.at(i)->getStart()->getLine());
    }
    lhs = yalll::Value(lhs.type_info,
                       builder->CreateAnd(lhs.get_llvm_val(*builder),
                                          rhs.get_llvm_val(*builder)), ctx->rhs.at(i)->getStart()->getLine());
  }

  return lhs;
}

std::any YALLLVisitorImpl::visitCompare_op(
    YALLLParser::Compare_opContext* ctx) {
  return visitChildren(ctx);
}

std::any YALLLVisitorImpl::visitAddition_op(
    YALLLParser::Addition_opContext* ctx) {
  if (ctx->rhs.size() == 0) return visit(ctx->lhs);

  yalll::Value lhs = to_value(visit(ctx->lhs));
  std::vector<yalll::Value> rhs;

  for (auto i = 0; i < ctx->op.size(); ++i) {
    auto op = ctx->op.at(i)->getType();
    rhs.push_back(to_value(visit(ctx->rhs.at(i))));

    if (!lhs.type_info.make_compatible(rhs.at(i).type_info)) {
      incompatible_types(lhs.type_info, rhs.at(i).type_info,
                         ctx->rhs.at(i)->getStart()->getLine());
    }

    // if (op == YALLLParser::PLUS_SYM) {
    //   lhs = yalll::Value(lhs.type_info,
    //                      builder->CreateAdd(lhs.get_llvm_val(*builder),
    //                                         rhs.get_llvm_val(*builder)));
    // } else if (op == YALLLParser::MINSU_SYM) {
    //   lhs = yalll::Value(lhs.type_info,
    //                      builder->CreateSub(lhs.get_llvm_val(*builder),
    //                                         rhs.get_llvm_val(*builder)));
    // } else {
    //   std::cout << "Invalid operant in addition " << op << std::endl;
    //   break;
    // }
  }

  return lhs;
}

std::any YALLLVisitorImpl::visitMultiplication_op(
    YALLLParser::Multiplication_opContext* ctx) {
  if (ctx->rhs.size() == 0) return visit(ctx->lhs);
  std::cout << ":" << std::endl;

  yalll::Value lhs = to_value(visit(ctx->lhs));
  std::cout << lhs.to_string() << std::endl;
  yalll::Value rhs;

  for (auto i = 0; i < ctx->op.size(); ++i) {
    auto op = ctx->op.at(i)->getType();
    rhs = to_value(visit(ctx->rhs.at(i)));

    if (!lhs.type_info.make_compatible(rhs.type_info)) {
      incompatible_types(lhs.type_info, rhs.type_info,
                         ctx->rhs.at(i)->getStart()->getLine());
      return lhs;
    }

    if (op == YALLLParser::MUL_SYM) {
      lhs = yalll::Value(lhs.type_info,
                         builder->CreateMul(lhs.get_llvm_val(*builder),
                                            rhs.get_llvm_val(*builder)), ctx->rhs.at(i)->getStart()->getLine());
    } else if (op == YALLLParser::DIV_SYM) {
      if (lhs.type_info.is_signed()) {
        lhs = yalll::Value(lhs.type_info,
                           builder->CreateSDiv(lhs.get_llvm_val(*builder),
                                               rhs.get_llvm_val(*builder)), ctx->getStart()->getLine());
      } else {
        lhs = yalll::Value(lhs.type_info,
                           builder->CreateUDiv(lhs.get_llvm_val(*builder),
                                               rhs.get_llvm_val(*builder)), ctx->getStart()->getLine());
      }
    } else if (op == YALLLParser::MOD_SYM) {
      if (lhs.type_info.is_signed()) {
        lhs = yalll::Value(lhs.type_info,
                           builder->CreateSRem(lhs.get_llvm_val(*builder),
                                               rhs.get_llvm_val(*builder)), ctx->getStart()->getLine());
      } else {
        lhs = yalll::Value(lhs.type_info,
                           builder->CreateURem(lhs.get_llvm_val(*builder),
                                               rhs.get_llvm_val(*builder)), ctx->getStart()->getLine());
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
  std::cout << "high" << std::endl;
  auto tmp = to_value(visit(ctx->val));
  std::cout << "high out" << std::endl;
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
      return yalll::Value(typesafety::TypeInformation::INTAUTO_T(*context),
                          ctx->val->getText(), ctx->val->getLine());

    case YALLLParser::NAME: {
      auto* value = cur_scope->find_field(ctx->val->getText());
      if (value) {
        return *value;
      } else {
        std::cout << "Undefined variable used " << ctx->val->getText()
                  << std::endl;
        return yalll::Value(typesafety::TypeInformation::VOID_T(*context),
                            llvm::PoisonValue::get(builder->getVoidTy()), ctx->val->getLine());
      }
    }

    case YALLLParser::DECIMAL:
      return yalll::Value(typesafety::TypeInformation::D32_T(*context),
                          ctx->val->getText(), ctx->val->getLine());

    case YALLLParser::BOOL_TRUE:
      return yalll::Value(typesafety::TypeInformation::BOOL_T(*context),
                          builder->getInt1(true), ctx->val->getLine());

    case YALLLParser::BOOL_FALSE:
      return yalll::Value(typesafety::TypeInformation::BOOL_T(*context),
                          builder->getInt1(false), ctx->val->getLine());

    case YALLLParser::NULL_VALUE:
      return yalll::Value::NULL_VALUE(*context, ctx->val->getLine());

    default:
      std::cout << "Unknown terminal type found " << ctx->val->getText()
                << std::endl;
      break;
  }

  return visitChildren(ctx);
}

}  // namespace yallc
