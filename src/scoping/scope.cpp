#include "scope.h"

#include <string>
#include <utility>

namespace scoping {

void Scope::push(std::string ctx_name) {
  std::cout << "Scope pushed" << std::endl;
  scope_frames.push_back(ScopeData());

  if (ctx_name != "") {
    scope_frames.back().ctx_name = (scope_frames.end() - 1)->ctx_name;
  } else {
    scope_frames.back().ctx_name = ctx_name;
  }
}

void Scope::pop() {
  if (scope_frames.size() == 1) {
    std::cout << "Cannot pop root scope" << std::endl;
  }

  scope_frames.pop_back();
  std::cout << "Scope poped" << std::endl;
}

void Scope::add_field(const std::string& name, yalll::Value&& value) {
  scope_frames.back().field_map.insert(
      std::pair<std::string, yalll::Value>(name, value));
}

void Scope::add_function(const std::string& name, yalll::Function&& func) {
  scope_frames.back().func_map.insert(
      std::pair<std::string, yalll::Function>(name, func));
}

yalll::Value* Scope::find_field(const std::string& name) {
  std::cout << "seraching " << name << std::endl;
  for (auto i = scope_frames.size() - 1; i >= 0; --i) {
    if (scope_frames.at(i).field_map.contains(name))
      return &scope_frames.at(i).field_map.at(name);
  }
  std::cout << name << " does not exist" << std::endl;
  return nullptr;
}

yalll::Function* Scope::find_function(const std::string& name) {
  for (auto i = scope_frames.size() - 1; i >= 0; --i) {
    if (scope_frames.at(i).func_map.contains(name))
      return &scope_frames.at(i).func_map.at(name);
  }

  std::cout << name << " does not exist" << std::endl;
  return nullptr;
}

std::string& Scope::get_scope_ctx_name() {
  return scope_frames.back().ctx_name;
}
}  // namespace scoping
