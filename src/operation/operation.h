#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include <cstddef>
#include <vector>

#include "../typesafety/typeresolver.h"
#include "../typesafety/typesafety.h"
#include "../value/value.h"

namespace yalll {

// I'm honestly not very happy with this solution, espacially the generate_value
// method but it is what it is right now. I will definetly rework this at some
// point.
class Operation {
 public:
  Operation() : operations(), op_codes() {}
  Operation(std::vector<std::shared_ptr<Operation>> operations,
            std::vector<size_t> op_codes)
      : operations(operations), op_codes(op_codes) {}
  Operation(Operation&& other)
      : operations(other.operations), op_codes(other.op_codes) {}
  Operation(const Operation& other)
      : operations(other.operations), op_codes(other.op_codes) {}
  Operation& operator=(Operation&& other);
  Operation& operator=(const Operation& other);
  virtual ~Operation() = default;

  std::vector<std::shared_ptr<Operation>>& get_values() { return operations; }
  std::vector<size_t>& get_ops() { return op_codes; }

  virtual Value generate_value();
  virtual std::vector<typesafety::TypeProposal> gather_and_resolve_proposals();

  bool resolve_with_type_info(typesafety::TypeInformation type_info);
  bool resolve_without_type_info();

 protected:
  std::vector<std::shared_ptr<Operation>> operations;
  std::vector<size_t> op_codes;
};

}  // namespace yalll
