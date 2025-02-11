#include "oroperation.h"

#include <llvm/IR/IRBuilder.h>

#include "../import/import.h"

namespace yalll {

Value OrOperation::generate_value() {
  Value lhs = operations.at(0)->generate_value();

  yalll::Import<llvm::IRBuilder<>> builder;
  for (auto i = 0; i < op_codes.size(); ++i) {
    auto rhs = operations[i + 1]->generate_value();
    lhs = Value(lhs.type_info,
                builder->CreateOr(lhs.get_llvm_val(), rhs.get_llvm_val()),
                lhs.get_line());
  }

  return std::move(lhs);
}
std::vector<typesafety::TypeProposal>
OrOperation::gather_and_resolve_proposals() {
  std::vector<typesafety::TypeProposal> proposals;
  for (auto op : operations) {
    auto tmp = op->gather_and_resolve_proposals();
    proposals.insert(proposals.end(), tmp.begin(), tmp.end());
  }
  auto bool_t = typesafety::TypeInformation::BOOL_T();
  if (typesafety::TypeResolver::try_resolve_to_type(proposals, bool_t)) {
    return std::move(std::vector<typesafety::TypeProposal>{
        typesafety::TypeProposal{YALLLParser::BOOL_T, true, nullptr}});
  } else {
    return std::move(std::vector<typesafety::TypeProposal>{});
  }
}
}  // namespace yalll
