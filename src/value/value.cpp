#include "value.h"

namespace yalll {

Value::Value(const Value& other) {
  name = other.name;
  llvm_val = other.llvm_val;
  type_info = other.type_info;
  named = other.named;
}

Value& Value::operator=(const Value& other) {
  if(this == &other) return *this;
  
  name = other.name;
  llvm_val = other.llvm_val;
  type_info = other.type_info;
  named = other.named;

  return *this;
}

Value::Value(Value&& other) {
  name = other.name;
  llvm_val = other.llvm_val;
  type_info = other.type_info;
  named = other.named;
}

Value& Value::operator=(Value&& other) {
  name = other.name;
  llvm_val = other.llvm_val;
  type_info = other.type_info;
  named = other.named;
  return *this;
}
}
