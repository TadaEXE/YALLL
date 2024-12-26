#pragma once

#include "YALLLBaseVisitor.h"
#include "YALLLParser.h"

namespace yalli {
class YALLLVisitor : public YALLLBaseVisitor {
 public:
  std::any visitEntry_point(YALLLParser::Entry_pointContext* ctx) override;

  std::any visitVar_def(YALLLParser::Var_defContext* ctx) override;

  std::any visitAddition_op(YALLLParser::Addition_opContext* ctx) override;
};
}  // namespace yalli
