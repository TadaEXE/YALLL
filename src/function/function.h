#pragma once

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

#include <string>
#include <vector>

#include "../typesafety/typesafety.h"
#include "../value/value.h"

namespace yalll {

class Function {
  Function(std::string name, typesafety::TypeInformation return_type,
           std::vector<yalll::Value> parameter_list)
      : name(name), return_type(return_type), parameter_list(parameter_list) {}

  llvm::Function* generate_function_sig(llvm::Module& module);

 private:
  std::vector<llvm::Type*> param_list_to_type_list();
  
  std::string name;
  typesafety::TypeInformation return_type;
  std::vector<yalll::Value> parameter_list;
};
}  // namespace yalll
