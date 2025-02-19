#include "typesafety.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/Casting.h>

#include <format>

#include "../import/import.h"
#include "../logging/logger.h"
#include "YALLLParser.h"
#include "compatibilitymatrix.h"
#include "typesizes.h"

namespace typesafety {

TypeInformation& TypeInformation::operator=(const TypeInformation& other) {
  if (this == &other) return *this;

  yalll_t = other.yalll_t;
  llvm_t = other.llvm_t;
  errable = other.errable;
  mutable_ = other.mutable_;

  return *this;
}

TypeInformation& TypeInformation::operator=(TypeInformation&& other) {
  yalll_t = other.yalll_t;
  llvm_t = other.llvm_t;
  errable = other.errable;
  mutable_ = other.mutable_;

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

TypeInformation TypeInformation::from_yalll_t(size_t yalll_t) {
  switch (yalll_t) {
    case YALLLParser::I8_T:
      return I8_T();
    case YALLLParser::I16_T:
      return I16_T();
    case YALLLParser::I32_T:
      return I32_T();
    case YALLLParser::I64_T:
      return I64_T();
    case YALLLParser::U8_T:
      return U8_T();
    case YALLLParser::U16_T:
      return U16_T();
    case YALLLParser::U32_T:
      return U32_T();
    case YALLLParser::U64_T:
      return U64_T();
    case YALLLParser::D32_T:
      return D32_T();
    case YALLLParser::D64_T:
      return D64_T();
    case YALLLParser::BOOL_T:
      return BOOL_T();
    case YALLLParser::VOID_T:
      return VOID_T();
    case INTAUTO_T_ID:
      return INTAUTO_T();
    case DECAUTO_T_ID:
      return DECAUTO_T();
    case YALLLParser::TBD_T:
      return TBD_T();
    default:
      yalll::Import<util::Logger> logger;
      logger->send_internal_error("Got unexpected yalll_t {} in from_yalll_t",
                                  yalll_t);
      return TBD_T();
  }
}

TypeInformation TypeInformation::from_context_node(
    YALLLParser::TypeContext* node) {
  auto type = from_yalll_t(node->ty->getStart()->getType());
  if (node->errable) type = type.make_errable();
  if (node->mutable_) type = type.make_mutable();
  return type;
}

TypeInformation& TypeInformation::make_mutable() {
  mutable_ = true;
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

bool TypeInformation::is_mutable() const { return mutable_; }

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
  std::string base_t;
  switch (yalll_t) {
    case YALLLParser::I8_T:
      base_t = "i8";
      break;
    case YALLLParser::I16_T:
      base_t = "i16";
      break;
    case YALLLParser::I32_T:
      base_t = "i32";
      break;
    case YALLLParser::I64_T:
      base_t = "i64";
      break;
    case YALLLParser::U8_T:
      base_t = "u8";
      break;
    case YALLLParser::U16_T:
      base_t = "u16";
      break;
    case YALLLParser::U32_T:
      base_t = "u32";
      break;
    case YALLLParser::U64_T:
      base_t = "u64";
      break;
    case YALLLParser::D32_T:
      base_t = "d32";
      break;
    case YALLLParser::D64_T:
      base_t = "d64";
      break;
    case YALLLParser::BOOL_T:
      base_t = "bool";
      break;
    case INTAUTO_T_ID:
      base_t = "integer";
      break;
    case DECAUTO_T_ID:
      base_t = "decimal";
      break;
    case YALLLParser::TBD_T:
      base_t = "tbd";
      break;

    default:
      base_t = std::format("Unknown {}", yalll_t);
      break;
  }

  return std::format("{}{}{}", mutable_ ? "!" : "", errable ? "?" : "", base_t);
}

}  // namespace typesafety
