#pragma once

#include <vector>

#include "../value/value.h"
#include "typesafety.h"
namespace typesafety {

class TypeResolver {
 public:
  static bool resolve(yalll::Value& lhs, std::vector<yalll::Value>& rhs);
  static bool is_strict_type(TypeInformation& type_info);
  static void unsave_multi_cast(TypeInformation& type, yalll::Value& lhs,
                         std::vector<yalll::Value>& rhs);
};
}  // namespace typesafety
