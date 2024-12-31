#pragma once

#include <llvm/IR/Value.h>

#include <map>
#include <string>
namespace scoping {

class Scope {
 public:
  Scope() : parent(this) {}
  explicit Scope(Scope* parent) : parent(parent) {}
  ~Scope();

  Scope* push();
  Scope* pop();

  void add_field(const std::string& name, llvm::Value* value);
  llvm::Value* find_field(const std::string& name);

 private:
  Scope* parent;
  Scope* child;

  void delete_child();

  std::map<std::string, llvm::Value*> field_map;
};
}  // namespace scoping
