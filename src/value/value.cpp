#include "value.h"

#include <llvm/ADT/APFloat.h>
#include <llvm/Support/raw_ostream.h>

#include <format>
#include <string>

namespace yalll {

Value::Value(const Value& other) {
  name = other.name;
  llvm_val = other.llvm_val;
  type_info = other.type_info;
  named = other.named;
  value_string = other.value_string;
  array = other.array;
}

Value& Value::operator=(const Value& other) {
  if (this == &other) return *this;

  name = other.name;
  llvm_val = other.llvm_val;
  type_info = other.type_info;
  named = other.named;
  value_string = other.value_string;
  array = other.array;

  return *this;
}

Value::Value(Value&& other) {
  name = other.name;
  llvm_val = other.llvm_val;
  type_info = other.type_info;
  named = other.named;
  value_string = other.value_string;
  array = other.array;
}

Value& Value::operator=(Value&& other) {
  name = other.name;
  llvm_val = other.llvm_val;
  type_info = other.type_info;
  named = other.named;
  value_string = other.value_string;
  array = other.array;
  return *this;
}

std::string Value::to_string() {
  std::string llvm_val_str;
  llvm::raw_string_ostream rso(llvm_val_str);
  if (llvm_val) llvm_val->print(rso);
  return std::format(
      "Name[{}] Type[{}] Value[{}] StringValue[{}] Mutable[{}] Errable[{}]",
      name, type_info.to_string(), rso.str(), value_string,
      type_info.is_mutable(), type_info.is_errable());
}

llvm::Value* Value::get_llvm_val() {
  if (llvm_val) return llvm_val;

  if (type_info.get_yalll_type() != typesafety::INTAUTO_T_ID &&
      type_info.get_yalll_type() != YALLLParser::TBD_T &&
      type_info.get_yalll_type() != typesafety::DECAUTO_T_ID) {
    logger->send_log("Converting: {} to {}", value_string,
                     type_info.to_string());

    yalll::Import<llvm::IRBuilder<>> builder;
    switch (type_info.get_yalll_type()) {
      case YALLLParser::I8_T:
        llvm_val = llvm::ConstantInt::getSigned(builder->getInt8Ty(),
                                                std::stoi(value_string));
        break;
      case YALLLParser::I16_T:
        llvm_val = llvm::ConstantInt::getSigned(builder->getInt16Ty(),
                                                std::stoi(value_string));
        break;
      case YALLLParser::I32_T:
        llvm_val = llvm::ConstantInt::getSigned(builder->getInt32Ty(),
                                                std::stol(value_string));
        break;
      case YALLLParser::I64_T:
        llvm_val = llvm::ConstantInt::getSigned(builder->getInt64Ty(),
                                                std::stoll(value_string));
        break;
      case YALLLParser::U8_T:
        llvm_val = builder->getInt8(std::stoul(value_string));
        break;
      case YALLLParser::U16_T:
        llvm_val = builder->getInt16(std::stoul(value_string));
        break;
      case YALLLParser::U32_T:
        llvm_val = builder->getInt32(std::stoull(value_string));
        break;
      case YALLLParser::U64_T:
        llvm_val = builder->getInt64(std::stoull(value_string));
        break;
      case YALLLParser::D32_T:
        llvm_val = static_cast<llvm::Value*>(llvm::ConstantFP::get(
            builder->getContext(), llvm::APFloat(std::stof(value_string))));
        break;
      case YALLLParser::D64_T:
        llvm_val = static_cast<llvm::Value*>(llvm::ConstantFP::get(
            builder->getContext(), llvm::APFloat(std::stod(value_string))));
        break;
    }
  }

  return llvm_val;
}

llvm::Value* Value::llvm_cast(typesafety::TypeInformation& type_info) {
  if (name.size() == 0) {
    this->type_info = type_info;

    yalll::Import<llvm::IRBuilder<>> builder;
    switch (type_info.get_yalll_type()) {
      case YALLLParser::I8_T:
        llvm_val = llvm::ConstantInt::getSigned(builder->getInt8Ty(),
                                                std::stoi(value_string));
        break;
      case YALLLParser::I16_T:
        llvm_val = llvm::ConstantInt::getSigned(builder->getInt16Ty(),
                                                std::stoi(value_string));
        break;
      case YALLLParser::I32_T:
        llvm_val = llvm::ConstantInt::getSigned(builder->getInt32Ty(),
                                                std::stol(value_string));
        break;
      case YALLLParser::I64_T:
        llvm_val = llvm::ConstantInt::getSigned(builder->getInt64Ty(),
                                                std::stoll(value_string));
        break;
      case YALLLParser::U8_T:
        llvm_val = builder->getInt8(std::stoul(value_string));
        break;
      case YALLLParser::U16_T:
        llvm_val = builder->getInt16(std::stoul(value_string));
        break;
      case YALLLParser::U32_T:
        llvm_val = builder->getInt32(std::stoull(value_string));
        break;
      case YALLLParser::U64_T:
        llvm_val = builder->getInt64(std::stoull(value_string));
        break;
      case YALLLParser::D32_T:
        llvm_val = static_cast<llvm::Value*>(llvm::ConstantFP::get(
            builder->getContext(), llvm::APFloat(std::stof(value_string))));
        break;
      case YALLLParser::D64_T:
        llvm_val = static_cast<llvm::Value*>(llvm::ConstantFP::get(
            builder->getContext(), llvm::APFloat(std::stod(value_string))));
        break;
    }

    return llvm_val;
  } else {
    logger->send_error("Real casting not supported yet");
    return nullptr;
  }
}

}  // namespace yalll
