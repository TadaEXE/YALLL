#pragma once

#include <llvm/IR/Value.h>

#include <unordered_map>
#include <string>
#include <vector>

#include "../value/value.h"
#include "../function/function.h"

namespace scoping {

struct ScopeData {
  std::unordered_map<std::string, yalll::Value> field_map;
  std::unordered_map<std::string, yalll::Function> func_map;
  std::string ctx_name;
};

class Scope {
 public:

  void push(std::string ctx_name = "");
  void pop();

  void add_field(const std::string& name, yalll::Value&& value);
  void add_function(const std::string& name, yalll::Function&& func);

  yalll::Value* find_field(const std::string& name);
  yalll::Function* find_function(const std::string& name);

  void set_active_function(const std::string& name);
  void no_active_function();
  bool has_active_function() { return active_function != nullptr; }
  yalll::Function* get_active_function() { return active_function; }

  std::string& get_scope_ctx_name();

 private:
  std::vector<ScopeData> scope_frames {ScopeData()};
  yalll::Import<util::Logger> logger;
  yalll::Function* active_function = nullptr;
};
}  // namespace scoping
