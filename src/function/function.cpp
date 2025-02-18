#include "function.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

#include "../import/import.h"
#include "../typesafety/typesafety.h"

namespace yalll {

Function& Function::operator=(const Function& other) {
  if (this == &other) return *this;

  name = other.name;
  return_type = other.return_type;
  parameter_list = other.parameter_list;
  ret_val = other.ret_val;
  elut_ptr = other.elut_ptr;
  llvm_func = other.llvm_func;
  noerr = other.noerr;

  return *this;
}

Function& Function::operator=(Function&& other) {
  name = other.name;
  return_type = other.return_type;
  parameter_list = other.parameter_list;
  ret_val = other.ret_val;
  elut_ptr = other.elut_ptr;
  llvm_func = other.llvm_func;
  noerr = other.noerr;

  return *this;
}

std::vector<llvm::Type*> Function::param_list_to_type_list() {
  std::vector<llvm::Type*> type_list;
  for (auto param : parameter_list) {
    type_list.push_back(param.type_info.get_llvm_type());
  }

  return std::move(type_list);
}

llvm::Function* Function::generate_function_sig(llvm::Module& module) {
  logger->send_log("Generating function sig for {}:{} in module {}", name,
                   return_type.to_string(), module.getName().str());
  auto type_list = param_list_to_type_list();

  llvm::FunctionType* function_type;
  if (noerr) {
    function_type =
        llvm::FunctionType::get(return_type.get_llvm_type(), type_list, false);
  } else {
    // add implicit pointer for return type
    type_list.insert(type_list.begin(),
                     llvm::PointerType::get(module.getContext(), 0));

    // for normal function return type is always pointer to ELUT
    function_type = llvm::FunctionType::get(
        llvm::PointerType::get(module.getContext(), 0), type_list, false);
  }

  llvm::Function* function = llvm::Function::Create(
      function_type, llvm::Function::ExternalLinkage, name, module);

  if (!noerr) {
    function->arg_begin()->setName("retptr");
  }

  // + 1 offset for implicit pointer to return value
  uint8_t offset = noerr ? 0 : 1;
  for (auto i = 0; i < parameter_list.size(); ++i) {
    (function->arg_begin() + offset + i)->setName(parameter_list.at(i).name);
    parameter_list.at(i).llvm_val = (function->arg_begin() + 1 + i);
  }
  logger->send_log("{} takes {} arguments and is {}", name,
                   function->arg_size() - offset, noerr ? "noerr" : "errable");

  Import<llvm::LLVMContext> context;
  Import<llvm::IRBuilder<>> builder;
  auto body = llvm::BasicBlock::Create(*context, "entry", function);
  builder->SetInsertPoint(body);

  if (!noerr)
    elut_ptr = builder->CreateAlloca(builder->getPtrTy(), nullptr, "elut_ptr");

  if (!function) {
    logger->send_internal_error("Failed to generate function sig for {}", name);
  }
  llvm_func = function;
  logger->send_log("llvm_func: {}; function: {}", llvm_func != nullptr,
                   function != nullptr);
  return function;
}

void Function::generate_function_return(llvm::Value* return_override) {
  if (!llvm_func) {
    logger->send_internal_error("Function sig for {} was never generated",
                                name);
    return;
  }

  Import<llvm::IRBuilder<>> builder;
  if (noerr) {
    builder->CreateRet(ret_val.get_llvm_val());
  } else {
    builder->CreateStore(ret_val.get_llvm_val(), llvm_func->arg_begin());
    if (elut_ptr) {
      auto tmp = builder->CreateLoad(builder->getPtrTy(), elut_ptr);
      builder->CreateRet(tmp);
    } else {
      builder->CreateRet(llvm::ConstantPointerNull::get(builder->getPtrTy()));
    }
  }
}

}  // namespace yalll
