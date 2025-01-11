#pragma once

#include <cstddef>
#include <vector>

#include "../typesafety/typesafety.h"
#include "../value/value.h"

namespace yalll {

class Operation {
 public:
  explicit Operation(yalll::Value value) : terminal_value(value), terminal(true) {}
  Operation(std::vector<Operation>& operations, std::vector<size_t>& op_codes)
      : operations(operations), op_codes(op_codes) {}
  Operation(Operation&& other) = default;
  Operation(const Operation& other) = default;
  Operation& operator=(Operation&& other) = default;
  Operation& operator=(const Operation& other) = default;

  std::vector<Operation>& get_values() { return operations; }
  std::vector<size_t>& get_ops() { return op_codes; }
  bool is_terminal() const { return terminal; }

  llvm::Value* generate_llvm_val();

  bool resolve_with_type_info(typesafety::TypeInformation type_info);
  bool resolve_without_type_info();

 private:
  std::vector<Operation> operations;
  std::vector<size_t> op_codes;

  yalll::Value terminal_value;
  bool terminal = false;
};

}  // namespace yalll
