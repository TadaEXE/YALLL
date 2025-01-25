#include "addoperation.h"

#include <llvm/IR/IRBuilder.h>

namespace yalll {

Value AddOperation::generate_value(llvm::IRBuilder<>& builder) {
  if (is_terminal()) return terminal_value;
  Value lhs = operations.at(0)->generate_value(builder);
  bool float_mode = lhs.type_info.is_float_type();
  bool signed_mode = lhs.type_info.is_signed();

  for (auto i = 0; i < op_codes.size(); ++i) {
    auto rhs = operations[i + 1]->generate_value(builder);
    switch (op_codes[i]) {
      case YALLLParser::PLUS_SYM:
        if (float_mode) {
          lhs = Value(lhs.type_info,
                      builder.CreateFAdd(lhs.get_llvm_val(), rhs.get_llvm_val()), builder,
                      lhs.get_line());
        } else {
          lhs = Value(lhs.type_info,
                      builder.CreateAdd(lhs.get_llvm_val(), rhs.get_llvm_val()), builder,
                      lhs.get_line());
        }
        break;
      case YALLLParser::MINSU_SYM:
        if (float_mode) {
          lhs = Value(lhs.type_info,
                      builder.CreateFSub(lhs.get_llvm_val(), rhs.get_llvm_val()), builder,
                      lhs.get_line());
        } else {
          lhs = Value(lhs.type_info,
                      builder.CreateSub(lhs.get_llvm_val(), rhs.get_llvm_val()), builder,
                      lhs.get_line());
        }
        break;
    }
  }
  return std::move(lhs);
}
}  // namespace yalll
