#include "typeresolver.h"

#include <llvm/IR/LLVMContext.h>

#include "YALLLParser.h"
#include "typesafety.h"
#include "typesizes.h"

namespace typesafety {
bool TypeResolver::try_resolve(std::vector<TypeProposal>& values) {
  if (values.size() == 0) return true;
  size_t biggest_type = values.at(0).yalll_type;
  bool fixed_proposal_found = false;

  if (!is_strict_type(biggest_type)) {
    size_t line =
        values.at(0).attached_value->get_line();  // if the biggest type is not
                                                  // strict we know that there
                                                  // has to be a value atteched
    for (auto val : values) {
      if (val.fixed) {
        biggest_type = val.yalll_type;
        fixed_proposal_found = true;
        break;
      } else if (is_strict_type(val.yalll_type)) {
        biggest_type = val.yalll_type;
        break;
      }
    }
    if (!is_strict_type(biggest_type)) {
      if (is_intauto_type(biggest_type)) {
        biggest_type =
            static_cast<size_t>(TypeResolver::DefaultYalllTypes::INT);
      } else {
        biggest_type =
            static_cast<size_t>(TypeResolver::DefaultYalllTypes::DEC);
      }
    } else if (!values.at(0).attached_value->type_info.is_compatible(
                   biggest_type)) {
      auto tmp = TypeInformation::from_yalll_t(biggest_type);
      incompatible_types(values.at(0).attached_value->type_info, tmp, line);
      return false;
    }
  }

  for (auto val : values) {
    if (TypeInformation::yalll_ts_compatible(biggest_type, val.yalll_type)) {
      if (!fixed_proposal_found &&
          type_size.at(val.yalll_type) > type_size.at(biggest_type)) {
        biggest_type = val.yalll_type;
      }
    } else {
      auto tmp = TypeInformation::from_yalll_t(biggest_type);
      incompatible_types(tmp, val.attached_value->type_info,
                         val.attached_value->get_line());
      return false;
    }
  }

  auto tmp = TypeInformation::from_yalll_t(biggest_type);
  unsave_multi_cast(tmp, values);

  return true;
}

bool TypeResolver::try_resolve_to_type(std::vector<TypeProposal>& values,
                                       TypeInformation& hint) {
  if (values.size() == 0) return true;

  for (auto val : values) {
    if (val.fixed && val.yalll_type != hint.get_yalll_type()) {
      auto tmp = TypeInformation::from_yalll_t(val.yalll_type);
      incompatible_types(tmp, hint, 0);
      return false;
    }
    if (!hint.is_compatible(val.yalll_type)) {
      auto tmp = TypeInformation::from_yalll_t(val.yalll_type);
      incompatible_types(tmp, hint, 0);
      return false;
    }
  }

  unsave_multi_cast(hint, values);
  return true;
}

bool TypeResolver::is_strict_type(size_t yalll_t) {
  if (yalll_t == YALLLParser::TBD_T || yalll_t == INTAUTO_T_ID ||
      yalll_t == DECAUTO_T_ID) {
    return false;
  }
  return true;
}

bool TypeResolver::is_intauto_type(size_t yalll_t) {
  return yalll_t == typesafety::INTAUTO_T_ID;
}

void TypeResolver::unsave_multi_cast(TypeInformation& type_info,
                                     std::vector<TypeProposal>& values) {
  for (auto val : values) {
    if (!val.fixed && val.attached_value)
      val.attached_value->type_info = type_info;
  }
}

}  // namespace typesafety
