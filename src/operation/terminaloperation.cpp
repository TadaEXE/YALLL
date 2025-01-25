#include "terminaloperation.h"

namespace yalll {

Value TerminalOperation::generate_value(llvm::IRBuilder<>& builder) {
  return terminal_value;
}

std::vector<typesafety::TypeProposal>
TerminalOperation::gather_and_resolve_proposals(llvm::LLVMContext& ctx) {
  return std::move(
      std::vector<typesafety::TypeProposal>{typesafety::TypeProposal{
          terminal_value.type_info.get_yalll_type(), false, &terminal_value}});
}
}  // namespace yalll
