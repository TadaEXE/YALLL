#include "addoperation.h"

#include <llvm/IR/IRBuilder.h>
#include "../import/import.h"

namespace yalll {

Value AddOperation::generate_value() {
  Value lhs = operations.at(0)->generate_value();
  bool float_mode = lhs.type_info.is_float_type();
  bool signed_mode = lhs.type_info.is_signed();

  yalll::Import<llvm::IRBuilder<>> builder;

  for (auto i = 0; i < op_codes.size(); ++i) {
    auto rhs = operations[i + 1]->generate_value();
    switch (op_codes[i]) {
      case YALLLParser::PLUS_SYM:
        if (float_mode) {
          lhs =
              Value(lhs.type_info,
                    builder->CreateFAdd(lhs.get_llvm_val(), rhs.get_llvm_val()),
                    lhs.get_line());
        } else {
          lhs = Value(lhs.type_info,
                      builder->CreateAdd(lhs.get_llvm_val(), rhs.get_llvm_val()),
                      lhs.get_line());
        }
        break;
      case YALLLParser::MINSU_SYM:
        if (float_mode) {
          lhs =
              Value(lhs.type_info,
                    builder->CreateFSub(lhs.get_llvm_val(), rhs.get_llvm_val()),
                    lhs.get_line());
        } else {
          lhs = Value(lhs.type_info,
                      builder->CreateSub(lhs.get_llvm_val(), rhs.get_llvm_val()),
                      lhs.get_line());
        }
        break;
    }
  }
  logger->send_log("GenAdd: {}", lhs.to_string());
  return std::move(lhs);
}

std::vector<typesafety::TypeProposal>
AddOperation::gather_and_resolve_proposals() {
  std::vector<typesafety::TypeProposal> proposals;
  for (auto op : operations) {
    auto tmp = op->gather_and_resolve_proposals();
    proposals.insert(proposals.end(), tmp.begin(), tmp.end());
  }

  return std::move(proposals);
}
}  // namespace yalll
