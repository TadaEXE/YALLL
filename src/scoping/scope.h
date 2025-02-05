#pragma once

#include <llvm/IR/Value.h>

#include <unordered_map>
#include <string>
#include <vector>

#include "../value/value.h"

namespace scoping {

struct ScopeData {
  std::unordered_map<std::string, yalll::Value> field_map;
  std::string ctx_name;
};

class Scope {
 public:

  void push(std::string ctx_name = "");
  void pop();

  void add_field(const std::string& name, yalll::Value&& value);
  yalll::Value* find_field(const std::string& name);
  std::string& get_scope_ctx_name();

 private:
  std::vector<ScopeData> scope_frames {ScopeData()};
};
}  // namespace scoping
