#include "typeresolver.h"

#include <llvm/IR/LLVMContext.h>

#include "YALLLParser.h"
#include "typesafety.h"

namespace typesafety {
bool TypeResolver::try_resolve(std::vector<yalll::Value*>& values,
                               llvm::LLVMContext& ctx) {
  if (values.size() == 0) return true;
  TypeInformation biggest_type = values.at(0)->type_info;

  if (!is_strict_type(biggest_type)) {
    size_t line;
    for (auto* val : values) {
      if (is_strict_type(val->type_info)) {
        biggest_type = val->type_info;
        line = val->get_line();
        break;
      }
    }
    if (!is_strict_type(biggest_type)) {
      if (is_intauto_type(biggest_type)) {
        biggest_type = typesafety::TypeInformation::from_yalll_t(
            static_cast<size_t>(TypeResolver::DefaultYalllTypes::INT), ctx);
      } else {
        biggest_type = typesafety::TypeInformation::from_yalll_t(
            static_cast<size_t>(TypeResolver::DefaultYalllTypes::DEC), ctx);
      }
    } else if (!values.at(0)->type_info.is_compatible(biggest_type)) {
      incompatible_types(values.at(0)->type_info, biggest_type, line);
      return false;
    }
  }

  for (auto* val : values) {
    if (biggest_type.is_compatible(val->type_info)) {
      if (val->type_info > biggest_type) {
        biggest_type = val->type_info;
      }
    } else {
      incompatible_types(biggest_type, val->type_info, val->get_line());
      return false;
    }
  }

  unsave_multi_cast(biggest_type, values);

  return true;
}

bool TypeResolver::try_resolve_to_type(std::vector<yalll::Value*>& values, llvm::LLVMContext& ctx, TypeInformation& hint) {
  if (values.size() == 0) return true;

  for (auto* val : values) {
    if (!hint.is_compatible(val->type_info)) {
      incompatible_types(val->type_info, hint, val->get_line());
      return false;
    }
  }

  unsave_multi_cast(hint, values);
  return true;
}

bool TypeResolver::is_strict_type(TypeInformation& type_info) {
  if (type_info.get_yalll_type() == YALLLParser::TBD_T ||
      type_info.get_yalll_type() == INTAUTO_T_ID ||
      type_info.get_yalll_type() == DECAUTO_T_ID) {
    return false;
  }
  return true;
}

bool TypeResolver::is_intauto_type(TypeInformation& type_info) {
  return type_info.get_yalll_type() == typesafety::INTAUTO_T_ID;
}

void TypeResolver::unsave_multi_cast(TypeInformation& type_info,
                                     std::vector<yalll::Value*>& values) {
  for (auto* val : values) {
    val->type_info = type_info;
  }
}

}  // namespace typesafety
