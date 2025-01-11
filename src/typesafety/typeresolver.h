#pragma once

#include <llvm/IR/LLVMContext.h>
#include <vector>

#include "../value/value.h"
#include "typesafety.h"
namespace typesafety {

class TypeResolver {
 public:
  // Will leave all values with the same real type that is not tbd
  // returns false if not possible (won't touch values then)
  static bool try_resolve(std::vector<yalll::Value>& values);
  static bool try_resolve(yalll::Value& lhs, std::vector<yalll::Value>& rhs);

  static bool try_resolve_temps(yalll::Value& lhs,
                                std::vector<yalll::Value>& rhs, llvm::LLVMContext& ctx);
  static bool try_partial_resolve(yalll::Value& lhs,
                                  std::vector<yalll::Value>& rhs);

 private:
  static std::vector<yalll::Value*>& get_temp_vals(
      yalll::Value& lhs, std::vector<yalll::Value>& rhs);
  static bool is_strict_type(TypeInformation& type_info);
  static bool is_temp_type(TypeInformation& type_info);
  static void unsave_multi_cast(TypeInformation& type, yalll::Value& lhs,
                                std::vector<yalll::Value>& rhs);
  static bool contains_dec_temp(std::vector<yalll::Value*>& temps);
  static TypeInformation* find_biggest_strict_type(yalll::Value& lhs, std::vector<yalll::Value>& rhs);
};
}  // namespace typesafety
