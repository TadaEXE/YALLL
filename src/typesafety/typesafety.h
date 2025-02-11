#pragma once

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <cstddef>
#include <map>

#include "../import/import.h"
#include "YALLLParser.h"

namespace typesafety {

constexpr size_t INTAUTO_T_ID = 42069;
constexpr size_t DECAUTO_T_ID = 133769;

class TypeInformation {
 public:
  TypeInformation() : llvm_t(), yalll_t(), immutalbe(true), errable(false) {}
  TypeInformation(const TypeInformation& other)
      : llvm_t(other.llvm_t),
        yalll_t(other.yalll_t),
        immutalbe(other.immutalbe),
        errable(other.errable) {}
  TypeInformation& operator=(const TypeInformation& other);
  TypeInformation(TypeInformation&& other)
      : llvm_t(other.llvm_t),
        yalll_t(other.yalll_t),
        immutalbe(other.immutalbe),
        errable(other.errable) {}
  TypeInformation& operator=(TypeInformation&& other);
  bool operator>(TypeInformation& other);
  bool operator<(TypeInformation& other);
  bool operator>=(TypeInformation& other);
  bool operator<=(TypeInformation& other);
  bool operator==(TypeInformation& other);

  TypeInformation(llvm::Type* llvm_t, size_t yalll_t)
      : llvm_t(llvm_t), yalll_t(yalll_t) {}

  static TypeInformation I8_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getInt8Ty(*ctx), YALLLParser::I8_T);
  }
  static TypeInformation I16_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getInt16Ty(*ctx), YALLLParser::I16_T);
  }
  static TypeInformation I32_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getInt32Ty(*ctx), YALLLParser::I32_T);
  }
  static TypeInformation I64_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getInt64Ty(*ctx), YALLLParser::I64_T);
  }
  static TypeInformation U8_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getInt8Ty(*ctx), YALLLParser::U8_T);
  }
  static TypeInformation U16_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getInt16Ty(*ctx), YALLLParser::U16_T);
  }
  static TypeInformation U32_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getInt32Ty(*ctx), YALLLParser::U32_T);
  }
  static TypeInformation U64_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getInt64Ty(*ctx), YALLLParser::U64_T);
  }
  static TypeInformation D32_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getFloatTy(*ctx), YALLLParser::D32_T);
  }
  static TypeInformation D64_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getDoubleTy(*ctx), YALLLParser::D64_T);
  }
  static TypeInformation BOOL_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getInt1Ty(*ctx), YALLLParser::BOOL_T);
  }
  static TypeInformation VOID_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getVoidTy(*ctx), YALLLParser::VOID_T);
  }
  static TypeInformation TBD_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getVoidTy(*ctx), YALLLParser::TBD_T);
  }
  static TypeInformation INTAUTO_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getInt32Ty(*ctx), INTAUTO_T_ID);
  }
  static TypeInformation DECAUTO_T() {
    yalll::Import<llvm::LLVMContext> ctx;
    return TypeInformation(llvm::Type::getFloatTy(*ctx), DECAUTO_T_ID);
  }

  static TypeInformation from_yalll_t(size_t yalll_t);

  static TypeInformation from_context_node(YALLLParser::TypeContext* node);

  TypeInformation& make_mutable();
  TypeInformation& make_errable();

  bool is_signed() const;
  bool is_mutable() const;
  bool is_errable() const;
  bool is_compatible(TypeInformation& other) const;
  bool is_compatible(size_t yalll_t) const;
  static bool yalll_ts_compatible(size_t lhs, size_t rhs);
  bool is_float_type() const;

  llvm::Type* get_llvm_type() const;
  size_t get_yalll_type() const { return yalll_t; }

  std::string to_string() const;

 private:
  llvm::Type* llvm_t;
  size_t yalll_t;

  bool immutalbe;
  bool errable;

  std::map<size_t, bool> yalll_t_signed_map = {
      {YALLLParser::I8_T, true},    {YALLLParser::I16_T, true},
      {YALLLParser::I32_T, true},   {YALLLParser::I64_T, true},
      {YALLLParser::U8_T, false},   {YALLLParser::U16_T, false},
      {YALLLParser::U32_T, false},  {YALLLParser::U64_T, false},
      {YALLLParser::BOOL_T, false}, {YALLLParser::D32_T, true},
      {YALLLParser::D64_T, true},   {YALLLParser::VOID_T, false},
      {YALLLParser::USYS_T, false}, {YALLLParser::ISYS_T, true},
      {INTAUTO_T_ID, true},         {DECAUTO_T_ID, true}};
};

}  // namespace typesafety
