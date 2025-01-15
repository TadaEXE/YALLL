#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include <cstddef>
#include <vector>

#include "../typesafety/typesafety.h"
#include "../value/value.h"

namespace yalll {

// I'm honestly not very happy with this solution, espacially the generate_value
// method but it is what it is right now. I will definetly rework this at some
// point.
class Operation {
 public:
  explicit Operation(yalll::Value value)
      : terminal_value(value), terminal(true) {}
  Operation(std::vector<Operation>& operations, std::vector<size_t>& op_codes)
      : operations(operations), op_codes(op_codes) {}
  Operation(Operation&& other)
      : operations(other.operations),
        op_codes(other.op_codes),
        terminal(other.terminal),
        terminal_value(other.terminal_value) {}
  Operation(const Operation& other)
      : operations(other.operations),
        op_codes(other.op_codes),
        terminal(other.terminal),
        terminal_value(other.terminal_value) {}
  Operation& operator=(Operation&& other);
  Operation& operator=(const Operation& other);
  virtual ~Operation() = default;

  std::vector<Operation>& get_values() { return operations; }
  std::vector<size_t>& get_ops() { return op_codes; }
  bool is_terminal() const { return terminal; }

  virtual Value generate_value(llvm::IRBuilder<>& builder);

  bool resolve_with_type_info(typesafety::TypeInformation type_info,
                              llvm::LLVMContext& ctx);
  bool resolve_without_type_info(llvm::LLVMContext& ctx);

 protected:
  std::vector<yalll::Value*> get_terminals();

  std::vector<Operation> operations;
  std::vector<size_t> op_codes;

  yalll::Value terminal_value;
  bool terminal = false;
};

}  // namespace yalll
