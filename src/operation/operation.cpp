#include "operation.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include <vector>

#include "../typesafety/typeresolver.h"

namespace yalll {

Operation& Operation::operator=(Operation&& other) {
  operations = other.operations;
  op_codes = other.op_codes;
  terminal = other.terminal;
  terminal_value = other.terminal_value;

  return *this;
}

Operation& Operation::operator=(const Operation& other) {
  if (this == &other) return *this;

  operations = other.operations;
  op_codes = other.op_codes;
  terminal = other.terminal;
  terminal_value = other.terminal_value;

  return *this;
}

Value Operation::generate_value(llvm::IRBuilder<>& builder) {
  if (is_terminal()) return terminal_value;
  return std::move(operations.at(0)->generate_value(builder));
}

bool Operation::resolve_with_type_info(typesafety::TypeInformation type_info,
                                       llvm::LLVMContext& ctx) {
  auto terminals = get_terminals();

  return typesafety::TypeResolver::try_resolve_to_type(terminals, ctx, type_info);
}

bool Operation::resolve_without_type_info(llvm::LLVMContext& ctx) {
  auto terminals = get_terminals();
  return typesafety::TypeResolver::try_resolve(terminals, ctx);
}

std::vector<yalll::Value*> Operation::get_terminals() {
  if (is_terminal()) return std::vector<yalll::Value*>{&terminal_value};

  std::vector<yalll::Value*> terminals;

  for (auto i = 0; i < operations.size(); ++i)
    if (operations.at(i)->is_terminal()) {
      terminals.push_back(&operations.at(i)->terminal_value);
    } else {
      std::vector<yalll::Value*> sub_op_terminals =
          operations.at(i)->get_terminals();
      if (sub_op_terminals.size() > 0)
        terminals.insert(terminals.end(), sub_op_terminals.begin(),
                         sub_op_terminals.end());
    }

  return terminals;
}

}  // namespace yalll
