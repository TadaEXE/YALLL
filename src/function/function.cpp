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
  type_list.insert(type_list.begin(),
                   llvm::PointerType::get(module.getContext(), 0));

  llvm::FunctionType* function_type = llvm::FunctionType::get(
      llvm::IntegerType::getInt32Ty(module.getContext()), type_list, false);

  llvm::Function* function = llvm::Function::Create(
      function_type, llvm::Function::ExternalLinkage, name, module);

  for (auto i = 0; i < parameter_list.size(); ++i) {
    // + 1 offset for implicit error value pointer as first argument
    (function->arg_begin() + 1 + i)->setName(parameter_list.at(i).name);
  }

  return function;
}

}  // namespace yalll
