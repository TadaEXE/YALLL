#pragma once

#include <llvm/IR/LLVMContext.h>

#include <source_location>
#include <vector>

#include "../value/value.h"
#include "YALLLParser.h"
#include "typesafety.h"
namespace typesafety {
static inline void incompatible_types(
    typesafety::TypeInformation& lhs, typesafety::TypeInformation& rhs,
    size_t line,
    std::source_location location = std::source_location::current()) {
  std::cout << "[" << location.file_name() << ":" << std::endl
            << location.function_name() << "@" << location.line() << "]"
            << std::endl
            << "Incompatible types " << lhs.to_string() << " and "
            << rhs.to_string() << " in line " << line << std::endl;
}

struct TypeProposal {
 public:
  size_t yalll_type;
  bool fixed;
  yalll::Value* attached_value;
};

class TypeResolver {
 public:
  // Will leave all values with the same real type that is not tbd
  // returns false if not possible (won't touch values then)
  static bool try_resolve(std::vector<TypeProposal>& values);

  static bool try_resolve_to_type(std::vector<TypeProposal>& values,
                                  TypeInformation& hint);

  enum class DefaultYalllTypes {
    INT = YALLLParser::I32_T,
    DEC = YALLLParser::D32_T,
  };

 private:
  static std::vector<yalll::Value*>& get_temp_vals(
      yalll::Value& lhs, std::vector<yalll::Value>& rhs);

  static bool is_strict_type(size_t yalll_t);
  static bool is_intauto_type(size_t yalll_t);

  static void unsave_multi_cast(TypeInformation& type_info,
                                std::vector<TypeProposal>& values);

  static inline bool hint_check(TypeInformation* hint, TypeInformation& type) {
    if (!hint) return true;

    return type.is_compatible(*hint);
  }
};
}  // namespace typesafety
