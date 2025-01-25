#include "oroperation.h"

#include <llvm/IR/IRBuilder.h>

namespace yalll {

Value OrOperation::generate_value(llvm::IRBuilder<>& builder) {
  Value lhs = operations.at(0)->generate_value(builder);

  for (auto i = 0; i < op_codes.size(); ++i) {
    auto rhs = operations[i + 1]->generate_value(builder);
    lhs = Value(lhs.type_info,
                builder.CreateOr(lhs.get_llvm_val(), rhs.get_llvm_val()),
                builder, lhs.get_line());
  }

  return std::move(lhs);
}
}  // namespace yalll
