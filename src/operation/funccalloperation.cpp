#include "funccalloperation.h"

#include <vector>

namespace yalll {

Value FuncCallOperation::generate_value() {
  logger->send_log("Generating function call for {}", func.get_name());
  if (func.is_noerr()) {
    std::vector<llvm::Value*> arguments;

    for (auto op : operations) {
      arguments.push_back(op->generate_value().get_llvm_val());
    }

    return std::move(Value(func.get_return_type(), func.ret_val.get_llvm_val(),
                           func.ret_val.get_line()));

  } else {
    auto retvalptr = builder->CreateAlloca(func.get_return_type().get_llvm_type(), nullptr, "retvalptr");
    std::vector<llvm::Value*> arguments{retvalptr};

    for (auto op : operations) {
      arguments.push_back(op->generate_value().get_llvm_val());
    }

    builder->CreateCall(func.llvm_func,
                        llvm::ArrayRef<llvm::Value*>{arguments});
    auto retval = builder->CreateLoad(func.get_return_type().get_llvm_type(), retvalptr);
    return std::move(Value(func.get_return_type(), retval, func.ret_val.get_line()));
  }
}

std::vector<typesafety::TypeProposal>
FuncCallOperation::gather_and_resolve_proposals() {
  std::vector<typesafety::TypeProposal> proposals;
  for (auto op : operations) {
    auto tmp = op->gather_and_resolve_proposals();
    proposals.insert(proposals.end(), tmp.begin(), tmp.end());
  }
  (void)typesafety::TypeResolver::try_resolve_to_type(proposals,
                                                      func.get_return_type());
  return std::move(
      std::vector<typesafety::TypeProposal>{typesafety::TypeProposal{
          func.get_return_type().get_yalll_type(), true, nullptr}});
}
}  // namespace yalll
