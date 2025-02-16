#include "muloperation.h"

#include <llvm/IR/IRBuilder.h>

#include "../import/import.h"

namespace yalll {

Value MulOperation::generate_value() {
  Value lhs = operations.at(0)->generate_value();
  bool float_mode = lhs.type_info.is_float_type();
  bool signed_mode = lhs.type_info.is_signed();

  yalll::Import<llvm::IRBuilder<>> builder;

  for (auto i = 0; i < op_codes.size(); ++i) {
    auto rhs = operations[i + 1]->generate_value();
    switch (op_codes[i]) {
      case YALLLParser::MUL_SYM:
        if (float_mode) {
          lhs =
              Value(lhs.type_info,
                    builder->CreateFMul(lhs.get_llvm_val(), rhs.get_llvm_val()),
                    lhs.get_line());
        } else {
          lhs = Value(lhs.type_info,
                      builder->CreateMul(lhs.get_llvm_val(), rhs.get_llvm_val()),
                      lhs.get_line());
        }
        break;
      case YALLLParser::DIV_SYM:
        if (float_mode) {
          lhs =
              Value(lhs.type_info,
                    builder->CreateFDiv(lhs.get_llvm_val(), rhs.get_llvm_val()),
                    lhs.get_line());
        } else {
          if (signed_mode) {
            lhs = Value(
                lhs.type_info,
                builder->CreateSDiv(lhs.get_llvm_val(), rhs.get_llvm_val()),
                lhs.get_line());
          } else {
            lhs = Value(
                lhs.type_info,
                builder->CreateUDiv(lhs.get_llvm_val(), rhs.get_llvm_val()),
                lhs.get_line());
          }
        }
        break;
      case YALLLParser::MOD_SYM:
        if (float_mode) {
          lhs =
              Value(lhs.type_info,
                    builder->CreateFRem(lhs.get_llvm_val(), rhs.get_llvm_val()),
                    lhs.get_line());
        } else {
          if (signed_mode) {
            lhs = Value(
                lhs.type_info,
                builder->CreateSRem(lhs.get_llvm_val(), rhs.get_llvm_val()),
                lhs.get_line());
          } else {
            lhs = Value(
                lhs.type_info,
                builder->CreateURem(lhs.get_llvm_val(), rhs.get_llvm_val()),
                lhs.get_line());
          }
        }
        break;
    }
  }
  logger->send_log("GenMul: {}", lhs.to_string());
  return std::move(lhs);
}

std::vector<typesafety::TypeProposal>
MulOperation::gather_and_resolve_proposals() {
  std::vector<typesafety::TypeProposal> proposals;
  for (auto op : operations) {
    auto tmp = op->gather_and_resolve_proposals();
    proposals.insert(proposals.end(), tmp.begin(), tmp.end());
  }

  return std::move(proposals);
}
}  // namespace yalll
