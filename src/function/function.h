#pragma once

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

#include <string>
#include <vector>

#include "../import/import.h"
#include "../logging/logger.h"
#include "../typesafety/typesafety.h"
#include "../value/value.h"

namespace yalll {

class Function {
 public:
  Function(std::string name, typesafety::TypeInformation return_type,
           std::vector<yalll::Value> parameter_list, bool noerr = false)
      : name(name),
        return_type(return_type),
        parameter_list(parameter_list),
        noerr(noerr) {}

  Function(std::string name, typesafety::TypeInformation return_type,
           bool noerr = false)
      : name(name), return_type(return_type), parameter_list(), noerr(noerr) {}

  Function(const Function& other)
      : name(other.name),
        return_type(other.return_type),
        ret_val(other.ret_val),
        elut_ptr(other.elut_ptr),
        llvm_func(other.llvm_func),
        parameter_list(other.parameter_list),
        noerr(other.noerr) {}

  Function(Function&& other)
      : name(other.name),
        return_type(other.return_type),
        ret_val(other.ret_val),
        elut_ptr(other.elut_ptr),
        llvm_func(other.llvm_func),
        parameter_list(other.parameter_list),
        noerr(other.noerr) {}

  Function& operator=(const Function& other);
  Function& operator=(Function&& other);
  ~Function() = default;

  llvm::Function* generate_function_sig(llvm::Module& module);
  void generate_function_return(llvm::Value* return_override = nullptr);

  std::vector<yalll::Value>& get_parameters() { return parameter_list; }
  std::string& get_name() { return name; }
  bool is_noerr() { return noerr; }

  typesafety::TypeInformation& get_return_type() { return return_type; }

  Value ret_val;
  llvm::Value* elut_ptr = nullptr;
  llvm::Function* llvm_func = nullptr;

 private:
  std::vector<llvm::Type*> param_list_to_type_list();

  Import<util::Logger> logger;

  std::string name;
  typesafety::TypeInformation return_type;
  std::vector<yalll::Value> parameter_list;
  bool noerr;
};
}  // namespace yalll
