#pragma once

#include <llvm/IR/LLVMContext.h>

#include <vector>

#include "../value/value.h"
#include "YALLLParser.h"
#include "typesafety.h"
namespace typesafety {
static inline void incompatible_types(typesafety::TypeInformation& lhs,
                                      typesafety::TypeInformation& rhs,
                                      size_t line) {
  std::cout << "Incompatible types " << lhs.to_string() << " and "
            << rhs.to_string() << " in line " << line << std::endl;
}

class TypeResolver {
 public:
  // Will leave all values with the same real type that is not tbd
  // returns false if not possible (won't touch values then)
  static bool try_resolve(std::vector<yalll::Value*>& values,
                          llvm::LLVMContext& ctx);

  static bool try_resolve_to_type(std::vector<yalll::Value*>& values, llvm::LLVMContext& ctx, TypeInformation& hint);

  enum class DefaultYalllTypes {
    INT = YALLLParser::I32_T,
    DEC = YALLLParser::D32_T,
  };

 private:
  static std::vector<yalll::Value*>& get_temp_vals(
      yalll::Value& lhs, std::vector<yalll::Value>& rhs);

  static bool is_strict_type(TypeInformation& type_info);
  static bool is_intauto_type(TypeInformation& type_info);

  static void unsave_multi_cast(TypeInformation& type_info,
                                std::vector<yalll::Value*>& values);

  static inline bool hint_check(TypeInformation* hint, TypeInformation& type) {
    if (!hint) return true;

    return type.is_compatible(*hint);
  }
};
}  // namespace typesafety
