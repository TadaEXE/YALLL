#include "scope.h"

#include <string>
#include <utility>

namespace scoping {

void Scope::push() {
  std::cout << "Scope pushed" << std::endl;
  scope_frames.push_back(ScopeData());
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

yalll::Value* Scope::find_field(const std::string& name) {
  std::cout << "seraching " << name << std::endl;
  for (auto i = scope_frames.size() - 1; i >= 0; --i) {
    if (scope_frames.at(i).field_map.contains(name))
      return &scope_frames.at(i).field_map.at(name);
  }
  std::cout << name << " does not exist" << std::endl;
  return nullptr;
}
}  // namespace scoping
