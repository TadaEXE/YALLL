#pragma once

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <cstddef>
#include <map>

#include "YALLLParser.h"

namespace typesafety {

class TypeInformation {
 public:
  TypeInformation() : llvm_t(), yalll_t() {}
  TypeInformation(const TypeInformation& other);
  TypeInformation& operator=(const TypeInformation& other);
  TypeInformation(TypeInformation&& other);
  TypeInformation& operator=(TypeInformation&& other);

  TypeInformation(llvm::Type* llvm_t, size_t yalll_t)
      : llvm_t(llvm_t), yalll_t(yalll_t) {}

  static TypeInformation EMPTY() { return TypeInformation(); }
  static TypeInformation I8_T(llvm::LLVMContext& ctx) {
    return TypeInformation(llvm::Type::getInt8Ty(ctx), YALLLParser::I8_T);
  }
  static TypeInformation I16_T(llvm::LLVMContext& ctx) {
    return TypeInformation(llvm::Type::getInt16Ty(ctx), YALLLParser::I16_T);
  }
  static TypeInformation I32_T(llvm::LLVMContext& ctx) {
    return TypeInformation(llvm::Type::getInt32Ty(ctx), YALLLParser::I32_T);
  }
  static TypeInformation I64_T(llvm::LLVMContext& ctx) {
    return TypeInformation(llvm::Type::getInt64Ty(ctx), YALLLParser::I64_T);
  }
  static TypeInformation U8_T(llvm::LLVMContext& ctx) {
    return TypeInformation(llvm::Type::getInt8Ty(ctx), YALLLParser::U8_T);
  }
  static TypeInformation U16_T(llvm::LLVMContext& ctx) {
    return TypeInformation(llvm::Type::getInt16Ty(ctx), YALLLParser::U16_T);
  }
  static TypeInformation U32_T(llvm::LLVMContext& ctx) {
    return TypeInformation(llvm::Type::getInt32Ty(ctx), YALLLParser::U32_T);
  }
  static TypeInformation U64_T(llvm::LLVMContext& ctx) {
    return TypeInformation(llvm::Type::getInt64Ty(ctx), YALLLParser::U64_T);
  }
  static TypeInformation D32_T(llvm::LLVMContext& ctx) {
    return TypeInformation(llvm::Type::getFloatTy(ctx), YALLLParser::D32_T);
  }
  static TypeInformation D64_T(llvm::LLVMContext& ctx) {
    return TypeInformation(llvm::Type::getDoubleTy(ctx), YALLLParser::D64_T);
  }
  static TypeInformation BOOL_T(llvm::LLVMContext& ctx) {
    return TypeInformation(llvm::Type::getInt1Ty(ctx), YALLLParser::BOOL_T);
  }

  static TypeInformation from_yalll_t(size_t yalll_t, llvm::LLVMContext& ctx);

  bool is_compatible(TypeInformation& other);

  bool is_signed();
  bool is_nullable();
  bool is_mutable();

  llvm::Type* get_llvm_type() const;
  size_t get_yalll_type() const;

  std::string to_string() const;

 private:
  llvm::Type* llvm_t;
  size_t yalll_t;

  std::map<size_t, bool> yalll_t_signed_map = {
      {YALLLParser::I8_T, true},   {YALLLParser::I16_T, true},
      {YALLLParser::I32_T, true},  {YALLLParser::I64_T, true},
      {YALLLParser::U8_T, false},  {YALLLParser::U16_T, false},
      {YALLLParser::U32_T, false}, {YALLLParser::U64_T, false},
      {YALLLParser::BOOL_T, true}, {YALLLParser::D32_T, true},
      {YALLLParser::D64_T, true},
  };
};

}  // namespace typesafety
