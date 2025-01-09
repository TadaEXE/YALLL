#include "typesafety.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/Casting.h>

#include <format>
#include <iostream>

#include "YALLLParser.h"
#include "compatibilitymatrix.h"
#include "typesizes.h"

namespace typesafety {

TypeInformation::TypeInformation(const TypeInformation& other)
    : llvm_t(other.llvm_t), yalll_t(other.yalll_t) {}

TypeInformation::TypeInformation(TypeInformation&& other)
    : llvm_t(other.llvm_t), yalll_t(other.yalll_t) {}

TypeInformation& TypeInformation::operator=(const TypeInformation& other) {
  if (this == &other) return *this;

  yalll_t = other.yalll_t;
  llvm_t = other.llvm_t;

  return *this;
}

TypeInformation& TypeInformation::operator=(TypeInformation&& other) {
  yalll_t = other.yalll_t;
  llvm_t = other.llvm_t;

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

TypeInformation& TypeInformation::make_mutable() {
  immutalbe = false;
  return *this;
}

TypeInformation& TypeInformation::make_nullable() {
  nullable = true;
  return *this;
}

// bool TypeInformation::make_compatible(TypeInformation& other) {
//   std::cout << this->to_string() << " <> " << other.to_string() << " -> ";
//
//   if (yalll_t == YALLLParser::TBD_T ^ other.yalll_t == YALLLParser::TBD_T) {
//     if (yalll_t == TBD_T_ID) {
//       *this = other;
//     } else {
//       other = *this;
//     }
//   }
//   if (yalll_t == INTAUTO_T_ID ^ other.yalll_t == INTAUTO_T_ID) {
//     if (yalll_t == INTAUTO_T_ID) {
//       *this = other;
//     } else {
//       other = *this;
//     }
//   }
//   if (yalll_t == DECAUTO_T_ID ^ other.yalll_t == DECAUTO_T_ID) {
//     if (yalll_t == DECAUTO_T_ID) {
//       *this = other;
//     } else {
//       other = *this;
//     }
//   }
//   std::cout << this->to_string() << " <> " << other.to_string() << std::endl;
//   return yalll_t == other.yalll_t &&
//          llvm_t->getTypeID() == other.llvm_t->getTypeID();
// }

bool TypeInformation::is_signed() const {
  if (yalll_t_signed_map.contains(yalll_t)) {
    return yalll_t_signed_map.at(yalll_t);
  }
  return true;
}

bool TypeInformation::is_nullable() const { return nullable; }

bool TypeInformation::is_mutable() const { return !immutalbe; }

bool TypeInformation::is_compatible(TypeInformation& other) const {
  return compatiblity_matrix.at(yalll_t).at(other.yalll_t);
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
    case TBD_T_ID:
      return "tbd";

    default:
      return std::format("Unknown {}", yalll_t);
  }
}

}  // namespace typesafety
