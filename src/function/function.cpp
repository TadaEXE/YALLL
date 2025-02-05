#include "function.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

namespace yalll {

std::vector<llvm::Type*> Function::param_list_to_type_list() {
  std::vector<llvm::Type*> type_list;
  for (auto param : parameter_list) {
    type_list.push_back(param.type_info.get_llvm_type());
  }

  return std::move(type_list);
}

llvm::Function* Function::generate_function_sig(llvm::Module& module) {
  auto type_list = param_list_to_type_list();
  // add implicit pointer for return type
  type_list.insert(type_list.begin(),
                   llvm::PointerType::get(module.getContext(), 0));

  // return type is always pointer to ELUT
  llvm::FunctionType* function_type = llvm::FunctionType::get(
      llvm::PointerType::get(module.getContext(), 0), type_list, false);

  llvm::Function* function = llvm::Function::Create(
      function_type, llvm::Function::ExternalLinkage, name, module);

  function->arg_begin()->setName("retptr");
  for (auto i = 0; i < parameter_list.size(); ++i) {
    // + 1 offset for implicit pointer to return value
    (function->arg_begin() + 1 + i)->setName(parameter_list.at(i).name);
  }

  return function;
}

}  // namespace yalll
