#pragma once

#include <llvm/IR/IRBuilder.h>

#include <vector>

#include "../function/function.h"
#include "operation.h"

namespace yalll {

class FuncCallOperation : public Operation {
 public:
  using Operation::Operation;
  explicit FuncCallOperation(Function& func,
                             std::vector<std::shared_ptr<Operation>> operations)
      : func(func), Operation(operations, std::vector<size_t>()) {}
  Value generate_value() override;
  std::vector<typesafety::TypeProposal> gather_and_resolve_proposals() override;

 private:
  Function& func;
  Import<llvm::IRBuilder<>> builder;
};
}  // namespace yalll
