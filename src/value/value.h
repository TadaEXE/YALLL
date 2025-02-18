#pragma once

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include <string>

#include "../import/import.h"
#include "../logging/logger.h"
#include "../typesafety/typesafety.h"

namespace yalll {

class Value {
 public:
  Value() : type_info(), llvm_val(), line(), name(), named(), value_string() {}

  Value(const Value& other);
  Value& operator=(const Value& other);
  Value(Value&& other);
  Value& operator=(Value&& other);

  Value(typesafety::TypeInformation type_info, llvm::Value* llvm_val,
        size_t line, std::string name = "")
      : type_info(type_info),
        llvm_val(llvm_val),
        line(line),
        name(name),
        named(!name.empty()),
        value_string("") {}

  Value(typesafety::TypeInformation type_info, std::string value_string,
        size_t line, std::string name = "")
      : type_info(type_info),
        value_string(value_string),
        line(line),
        name(name),
        named(!name.empty()),
        llvm_val(nullptr) {}

  static Value NULL_VALUE(size_t line) {
    yalll::Import<llvm::LLVMContext> context;
    auto tmp = Value(typesafety::TypeInformation::TBD_T(),
                     static_cast<llvm::Value*>(llvm::Constant::getNullValue(
                         llvm::Type::getVoidTy(*context))),
                     line);
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

  llvm::Value* llvm_val = nullptr;

 private:
  yalll::Import<util::Logger> logger;
  std::string value_string;

  size_t line;

  bool named;
  bool null_value = false;
};
}  // namespace yalll
