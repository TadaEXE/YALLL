#include "typesafety.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/Casting.h>

#include <format>

#include "YALLLParser.h"
#include "compatibilitymatrix.h"
#include "typesizes.h"

namespace typesafety {

TypeInformation& TypeInformation::operator=(const TypeInformation& other) {
  if (this == &other) return *this;

  yalll_t = other.yalll_t;
  llvm_t = other.llvm_t;
  errable = other.errable;
  immutalbe = other.immutalbe;

  return *this;
}

TypeInformation& TypeInformation::operator=(TypeInformation&& other) {
  yalll_t = other.yalll_t;
  llvm_t = other.llvm_t;
  errable = other.errable;
  immutalbe = other.immutalbe;

  return *this;
}

bool TypeInformation::operator>(TypeInformation& other) {
  if (compatiblity_matrix.at(yalll_t).at(other.yalll_t)) {
    return type_size.at(yalll_t) > type_size.at(other.yalll_t);
  }
  throw "Don't try to compare incompatible types please.";
}

bool TypeInformation::operator<(TypeInformation& other) {
  return !((*this) >= other);
}

bool TypeInformation::operator>=(TypeInformation& other) {
  if (compatiblity_matrix.at(yalll_t).at(other.yalll_t)) {
    return type_size.at(yalll_t) >= type_size.at(other.yalll_t);
  }
  throw "Don't try to compare incomaptible types please.";
}

bool TypeInformation::operator<=(TypeInformation& other) {
  return !((*this) > other);
}

bool TypeInformation::operator==(TypeInformation& other) {
  if (compatiblity_matrix.at(yalll_t).at(other.yalll_t)) {
    return type_size.at(yalll_t) == type_size.at(other.yalll_t);
  }
  throw "Don't try to compare incompatible types please.";
}

TypeInformation TypeInformation::from_yalll_t(size_t yalll_t,
                                              llvm::LLVMContext& ctx) {
  switch (yalll_t) {
    case YALLLParser::I8_T:
      return I8_T(ctx);
    case YALLLParser::I16_T:
      return I16_T(ctx);
    case YALLLParser::I32_T:
      return I32_T(ctx);
    case YALLLParser::I64_T:
      return I64_T(ctx);
    case YALLLParser::U8_T:
      return U8_T(ctx);
    case YALLLParser::U16_T:
      return U16_T(ctx);
    case YALLLParser::U32_T:
      return U32_T(ctx);
    case YALLLParser::U64_T:
      return U64_T(ctx);
    case YALLLParser::D32_T:
      return D32_T(ctx);
    case YALLLParser::D64_T:
      return D64_T(ctx);
    case YALLLParser::BOOL_T:
      return BOOL_T(ctx);
    case YALLLParser::VOID_T:
      return VOID_T(ctx);
    default:
      return TBD_T(ctx);
  }
}

TypeInformation TypeInformation::from_context_node(
    YALLLParser::TypeContext* node, llvm::LLVMContext& ctx) {
  auto type = from_yalll_t(node->ty->getStart()->getType(), ctx);
  if (node->errable) type = type.make_errable();
  if (node->mutable_) type = type.make_mutable();
  return type;
}

TypeInformation& TypeInformation::make_mutable() {
  immutalbe = false;
  return *this;
}

TypeInformation& TypeInformation::make_errable() {
  errable = true;
  return *this;
}

bool TypeInformation::is_signed() const {
  if (yalll_t_signed_map.contains(yalll_t)) {
    return yalll_t_signed_map.at(yalll_t);
  }
  return true;
}

bool TypeInformation::is_errable() const { return errable; }

bool TypeInformation::is_mutable() const { return !immutalbe; }

bool TypeInformation::is_compatible(TypeInformation& other) const {
  return compatiblity_matrix.at(yalll_t).at(other.yalll_t);
}

bool TypeInformation::is_compatible(size_t yalll_t) const {
  return compatiblity_matrix.at(this->yalll_t).at(yalll_t);
}

bool TypeInformation::yalll_ts_compatible(size_t lhs, size_t rhs) {
  if (compatiblity_matrix.contains(lhs) &&
      compatiblity_matrix.at(lhs).contains(rhs))
    return compatiblity_matrix.at(lhs).at(rhs);
  return false;
}

bool TypeInformation::is_float_type() const {
  return yalll_t == YALLLParser::D32_T || yalll_t == YALLLParser::D64_T;
}

std::string TypeInformation::to_string() const {
  switch (yalll_t) {
    case YALLLParser::I8_T:
      return "i8";
    case YALLLParser::I16_T:
      return "i16";
    case YALLLParser::I32_T:
      return "i32";
    case YALLLParser::I64_T:
      return "i64";
    case YALLLParser::U8_T:
      return "u8";
    case YALLLParser::U16_T:
      return "u16";
    case YALLLParser::U32_T:
      return "u32";
    case YALLLParser::U64_T:
      return "u64";
    case YALLLParser::D32_T:
      return "d32";
    case YALLLParser::D64_T:
      return "d64";
    case YALLLParser::BOOL_T:
      return "bool";
    case INTAUTO_T_ID:
      return "integer";
    case DECAUTO_T_ID:
      return "decimal";
    case YALLLParser::TBD_T:
      return "tbd";

    default:
      return std::format("Unknown {}", yalll_t);
  }
}

}  // namespace typesafety
