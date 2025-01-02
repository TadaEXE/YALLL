#include "scope.h"

#include <iostream>
#include <string>
#include <utility>

namespace scoping {

Scope::~Scope() {
  if (child) {
    delete child;
  }
}

Scope* Scope::push() {
  child = new Scope(this);
  return child;
}

void Scope::delete_child() {
  if (child) {
    delete child;
  }
}

Scope* Scope::pop() {
  if (parent == this) return this;

  parent->delete_child();
  return parent;
}

void Scope::add_field(const std::string& name, yalll::Value&& value) {
  field_map.insert(std::pair<std::string, yalll::Value>(name, value));
}

yalll::Value* Scope::find_field(const std::string& name) {
  if (field_map.contains(name)) {
    return &field_map.at(name);
  }

  if (parent == this) return nullptr;

  return parent->find_field(name);
}

}  // namespace scoping
