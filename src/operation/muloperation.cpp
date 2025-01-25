#include "muloperation.h"

#include <llvm/IR/IRBuilder.h>

namespace yalll {

Value MulOperation::generate_value(llvm::IRBuilder<>& builder) {
  if (is_terminal()) return terminal_value;
  Value lhs = operations.at(0)->generate_value(builder);
  bool float_mode = lhs.type_info.is_float_type();
  bool signed_mode = lhs.type_info.is_signed();

  for (auto i = 0; i < op_codes.size(); ++i) {
    auto rhs = operations[i + 1]->generate_value(builder);
    switch (op_codes[i]) {
      case YALLLParser::MUL_SYM:
        if (float_mode) {
          lhs = Value(lhs.type_info,
                      builder.CreateFMul(lhs.get_llvm_val(), rhs.get_llvm_val()), builder,
                      lhs.get_line());
        } else {
          lhs = Value(lhs.type_info,
                      builder.CreateMul(lhs.get_llvm_val(), rhs.get_llvm_val()), builder,
                      lhs.get_line());
        }
        break;
      case YALLLParser::DIV_SYM:
        if (float_mode) {
          lhs = Value(lhs.type_info,
                      builder.CreateFDiv(lhs.get_llvm_val(), rhs.get_llvm_val()), builder,
                      lhs.get_line());
        } else {
          if (signed_mode) {
            lhs = Value(lhs.type_info,
                        builder.CreateSDiv(lhs.get_llvm_val(), rhs.get_llvm_val()), builder,
                        lhs.get_line());
          } else {
            lhs = Value(lhs.type_info,
                        builder.CreateUDiv(lhs.get_llvm_val(), rhs.get_llvm_val()), builder,
                        lhs.get_line());
          }
        }
        break;
      case YALLLParser::MOD_SYM:
        if (float_mode) {
          lhs = Value(lhs.type_info,
                      builder.CreateFRem(lhs.get_llvm_val(), rhs.get_llvm_val()), builder,
                      lhs.get_line());
        } else {
          if (signed_mode) {
            lhs = Value(lhs.type_info,
                        builder.CreateSRem(lhs.get_llvm_val(), rhs.get_llvm_val()), builder,
                        lhs.get_line());
          } else {
            lhs = Value(lhs.type_info,
                        builder.CreateURem(lhs.get_llvm_val(), rhs.get_llvm_val()), builder,
                        lhs.get_line());
          }
        }
        break;
    }
  }
  return std::move(lhs);
}
}  // namespace yalll
