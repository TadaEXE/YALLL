#include "operation.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include <vector>

#include "../typesafety/typeresolver.h"

namespace yalll {

Operation& Operation::operator=(Operation&& other) {
  operations = other.operations;
  op_codes = other.op_codes;

  return *this;
}

Operation& Operation::operator=(const Operation& other) {
  if (this == &other) return *this;

  operations = other.operations;
  op_codes = other.op_codes;

  return *this;
}

Value Operation::generate_value(llvm::IRBuilder<>& builder) {
  if (operations.size() > 1) {
    std::cout << "Invalid top level operation" << std::endl;
  }
  return std::move(operations.at(0)->generate_value(builder));
}

std::vector<typesafety::TypeProposal> Operation::gather_and_resolve_proposals(
    llvm::LLVMContext& ctx) {
  std::vector<typesafety::TypeProposal> proposals;
  for (auto op : operations) {
    auto tmp = op->gather_and_resolve_proposals(ctx);
    proposals.insert(proposals.end(), tmp.begin(), tmp.end());
  }
  return std::move(proposals);
}

bool Operation::resolve_with_type_info(typesafety::TypeInformation type_info,
                                       llvm::LLVMContext& ctx) {
  auto proposals = gather_and_resolve_proposals(ctx);
  return typesafety::TypeResolver::try_resolve_to_type(proposals, ctx,
                                                       type_info);
}

bool Operation::resolve_without_type_info(llvm::LLVMContext& ctx) {
  auto proposals = gather_and_resolve_proposals(ctx);
  return typesafety::TypeResolver::try_resolve(proposals, ctx);
}

}  // namespace yalll
