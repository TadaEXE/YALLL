#pragma once

#include <llvm/IR/IRBuilder.h>

#include "operation.h"

namespace yalll {

class OrOperation : public Operation {
 public:
  using Operation::Operation;
  Value generate_value(llvm::IRBuilder<>& builder) override;
  std::vector<typesafety::TypeProposal> gather_and_resolve_proposals(
      llvm::LLVMContext& ctx) override;
};
}  // namespace yalll
