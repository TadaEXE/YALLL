#pragma once

#include "operation.h"

namespace yalll {

class TerminalOperation : public Operation {
 public:
  using Operation::Operation;
  explicit TerminalOperation(yalll::Value value)
      : terminal_value(value), terminal(true) {}
  Value generate_value(llvm::IRBuilder<>& builder) override;
  std::vector<typesafety::TypeProposal> gather_and_resolve_proposals(
      llvm::LLVMContext& ctx) override;

 private:
  yalll::Value terminal_value;
  bool terminal = false;
};
}  // namespace yalll
