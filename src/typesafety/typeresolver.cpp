#include "typeresolver.h"

#include "YALLLParser.h"
#include "typesafety.h"

namespace typesafety {
inline void incompatible_types(typesafety::TypeInformation& lhs,
                               typesafety::TypeInformation& rhs, size_t line) {
  std::cout << "Incompatible types " << lhs.to_string() << " and "
            << rhs.to_string() << " in line " << line << std::endl;
}

bool TypeResolver::resolve(yalll::Value& lhs, std::vector<yalll::Value>& rhs) {
  TypeInformation biggest_type = lhs.type_info;

  if (!is_strict_type(biggest_type)) {
    size_t line;
    for (yalll::Value val : rhs) {
      if (is_strict_type(val.type_info)) {
        biggest_type = val.type_info;
        line = val.get_line();
        break;
      }
    }
    if (!is_strict_type(biggest_type)) {
      return false;
    } else if (!lhs.type_info.is_compatible(biggest_type)) {
      incompatible_types(lhs.type_info, biggest_type, line);
      return false;
    }
  }

  for (yalll::Value val : rhs) {
    if (biggest_type.is_compatible(val.type_info)) {
      if (val.type_info > biggest_type) {
        biggest_type = val.type_info;
      }
    } else {
      incompatible_types(biggest_type, val.type_info, val.get_line());
      return false;
    }
  }
  unsave_multi_cast(biggest_type, lhs, rhs);
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

void TypeResolver::unsave_multi_cast(TypeInformation& type, yalll::Value& lhs,
                                     std::vector<yalll::Value>& rhs) {
  lhs.type_info = type;
  for (yalll::Value val : rhs) {
    val.type_info = type;
  }
}

}  // namespace typesafety
