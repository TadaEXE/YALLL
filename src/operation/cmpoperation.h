#pragma once

#include <llvm/IR/IRBuilder.h>

#include "../typesafety/typeresolver.h"
#include "operation.h"

namespace yalll {

class CmpOperation : public Operation {
 public:
  using Operation::Operation;
  Value generate_value() override;
  std::vector<typesafety::TypeProposal> gather_and_resolve_proposals() override;
};
}  // namespace yalll
