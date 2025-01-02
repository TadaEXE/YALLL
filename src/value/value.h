#pragma once

#include <llvm/IR/Value.h>

#include <string>

#include "../typesafety/typesafety.h"

namespace yalll {

class Value {
 public:
  Value() : name(), llvm_val(), type_info(), named() {}
  Value(const Value& other);
  Value& operator=(const Value& other);
  Value(Value&& other);
  Value& operator=(Value&& other);

  Value(llvm::Value* llvm_val, typesafety::TypeInformation type_info)
      : named(false), llvm_val(llvm_val), type_info(type_info) {};

  Value(std::string& name, llvm::Value* llvm_val,
        typesafety::TypeInformation type_info)
      : name(name), named(true), llvm_val(llvm_val), type_info(type_info) {};

  Value(std::string& name, typesafety::TypeInformation type_info)
      : name(name), named(true), llvm_val(nullptr), type_info(type_info) {}

  static Value EMPTY() {
    return Value(nullptr, typesafety::TypeInformation::EMPTY());
  }

  static Value EMPTY(std::string& name) {
    return Value(name, typesafety::TypeInformation::EMPTY());
  }

  std::string name;
  llvm::Value* llvm_val;
  typesafety::TypeInformation type_info;

  bool named;
};
}  // namespace yalll
