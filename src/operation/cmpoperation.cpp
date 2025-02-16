#include "cmpoperation.h"

#include <llvm/IR/IRBuilder.h>

#include "../import/import.h"

namespace yalll {

Value CmpOperation::generate_value() {
  Value lhs = operations.at(0)->generate_value();
  bool float_mode = lhs.type_info.is_float_type();
  bool signed_mode = lhs.type_info.is_signed();

  std::vector<llvm::Value*> cmps;

  yalll::Import<llvm::IRBuilder<>> builder;
  for (auto i = 0; i < op_codes.size(); ++i) {
    auto rhs = operations[i + 1]->generate_value();

    switch (op_codes[i]) {
      case YALLLParser::GREATER_SYM:
        if (float_mode) {
          cmps.push_back(
              builder->CreateFCmpUGT(lhs.get_llvm_val(), rhs.get_llvm_val()));
        } else {
          if (signed_mode) {
            cmps.push_back(
                builder->CreateICmpSGT(lhs.get_llvm_val(), rhs.get_llvm_val()));
          } else {
            cmps.push_back(
                builder->CreateICmpUGT(lhs.get_llvm_val(), rhs.get_llvm_val()));
          }
        }
        break;
      case YALLLParser::GREATER_EQUAL_SYM:
        if (float_mode) {
          cmps.push_back(
              builder->CreateFCmpUGE(lhs.get_llvm_val(), rhs.get_llvm_val()));
        } else {
          if (signed_mode) {
            cmps.push_back(
                builder->CreateICmpSGE(lhs.get_llvm_val(), rhs.get_llvm_val()));
          } else {
            cmps.push_back(
                builder->CreateICmpUGE(lhs.get_llvm_val(), rhs.get_llvm_val()));
          }
        }
        break;
      case YALLLParser::LESS_SYM:
        if (float_mode) {
          cmps.push_back(
              builder->CreateFCmpULT(lhs.get_llvm_val(), rhs.get_llvm_val()));
        } else {
          if (signed_mode) {
            cmps.push_back(
                builder->CreateICmpSLT(lhs.get_llvm_val(), rhs.get_llvm_val()));
          } else {
            cmps.push_back(
                builder->CreateICmpULT(lhs.get_llvm_val(), rhs.get_llvm_val()));
          }
        }
        break;
      case YALLLParser::LESS_EQUAL_SYM:
        if (float_mode) {
          cmps.push_back(
              builder->CreateFCmpULE(lhs.get_llvm_val(), rhs.get_llvm_val()));
        } else {
          if (signed_mode) {
            cmps.push_back(
                builder->CreateICmpSLE(lhs.get_llvm_val(), rhs.get_llvm_val()));
          } else {
            cmps.push_back(
                builder->CreateICmpULE(lhs.get_llvm_val(), rhs.get_llvm_val()));
          }
        }
        break;
      case YALLLParser::EQUAL_EQUAL_SYM:
        if (float_mode) {
          cmps.push_back(
              builder->CreateFCmpUEQ(lhs.get_llvm_val(), rhs.get_llvm_val()));
        } else {
          cmps.push_back(
              builder->CreateICmpEQ(lhs.get_llvm_val(), rhs.get_llvm_val()));
        }
        break;
      case YALLLParser::NOT_EQUAL_SYM:
        if (float_mode) {
          cmps.push_back(
              builder->CreateFCmpUNE(lhs.get_llvm_val(), rhs.get_llvm_val()));
        } else {
          cmps.push_back(
              builder->CreateICmpNE(lhs.get_llvm_val(), rhs.get_llvm_val()));
        }
        break;
    }

    lhs = rhs;
  }

  Value result(typesafety::TypeInformation::BOOL_T(), cmps.at(0),
               lhs.get_line());
  if (cmps.size() > 1) {
    for (auto* cmp : std::vector<llvm::Value*>(cmps.begin() + 1, cmps.end())) {
      result = Value(result.type_info,
                     builder->CreateAnd(result.get_llvm_val(), cmp),
                     result.get_line());
    }
  }


  logger->send_log("GenCmp: {}", lhs.to_string());
  return std::move(result);
}

std::vector<typesafety::TypeProposal>
CmpOperation::gather_and_resolve_proposals() {
  std::vector<typesafety::TypeProposal> proposals;
  for (auto op : operations) {
    auto tmp = op->gather_and_resolve_proposals();
    proposals.insert(proposals.end(), tmp.begin(), tmp.end());
  }

  if (typesafety::TypeResolver::try_resolve(proposals))
    return std::move(std::vector<typesafety::TypeProposal>{
        typesafety::TypeProposal{YALLLParser::BOOL_T, true, nullptr}});
  else
    return std::move(std::vector<typesafety::TypeProposal>{});
}

}  // namespace yalll
