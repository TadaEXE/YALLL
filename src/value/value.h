#pragma once

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include <string>

#include "../typesafety/typesafety.h"

namespace yalll {

class Value {
 public:
  Value()
      : type_info(),
        llvm_val(),
        builder(),
        line(),
        name(),
        named(),
        value_string() {}

  Value(const Value& other);
  Value& operator=(const Value& other);
  Value(Value&& other);
  Value& operator=(Value&& other);

  Value(typesafety::TypeInformation type_info, llvm::Value* llvm_val,
        llvm::IRBuilder<>& builder, size_t line, std::string name = "")
      : type_info(type_info),
        llvm_val(llvm_val),
        builder(&builder),
        line(line),
        name(name),
        named(!name.empty()),
        value_string("") {}

  Value(typesafety::TypeInformation type_info, std::string value_string,
        llvm::IRBuilder<>& builder, size_t line, std::string name = "")
      : type_info(type_info),
        value_string(value_string),
        builder(&builder),
        line(line),
        name(name),
        named(!name.empty()),
        llvm_val(nullptr) {}

  static Value NULL_VALUE(llvm::IRBuilder<>& builder, size_t line) {
    auto tmp = Value(typesafety::TypeInformation::TBD_T(builder.getContext()),
                     static_cast<llvm::Value*>(llvm::Constant::getNullValue(
                         llvm::Type::getVoidTy(builder.getContext()))),
                     builder, line);
    tmp.null_value = true;
    return tmp;
  }

  std::string to_string();

  std::string name;
  llvm::Value* get_llvm_val();
  typesafety::TypeInformation type_info;

  bool is_null() const { return null_value; }

  size_t get_line() const { return line; }

  llvm::Value* llvm_cast(typesafety::TypeInformation& type_info);

  llvm::Value* llvm_val;

 private:
  std::string value_string;
  llvm::IRBuilder<>* builder;

  size_t line;

  bool named;
  bool null_value = false;
};
}  // namespace yalll
