#include "scope.h"

#include <string>
#include <utility>

namespace scoping {

void Scope::push(std::string ctx_name) {
  logger->send_log("Scope pushed");
  scope_frames.push_back(ScopeData());

  if (ctx_name != "") {
    scope_frames.back().ctx_name = (scope_frames.end() - 1)->ctx_name;
  } else {
    scope_frames.back().ctx_name = ctx_name;
  }
}

void Scope::pop() {
  if (scope_frames.size() == 1) {
    logger->send_internal_error("Cannot pop root scope");
  }

  scope_frames.pop_back();
  logger->send_log("Scope poped");
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
  logger->send_log("Searching for variable {}", name);
  if (active_function) {
    logger->send_log("Searching in active function {}", active_function->get_name());
    auto& params = active_function->get_parameters();
    for (auto& param : params) {
      if (param.name == name) {
        logger->send_log("Paramert {} found", name);
        return &param;
      }
    }
  }

  for (auto i = scope_frames.size() - 1; i >= 0; --i) {
    if (scope_frames.at(i).field_map.contains(name)) {
      logger->send_log("Variable {} found", name);
      return &scope_frames.at(i).field_map.at(name);
    }
  }
  logger->send_error("{} does not exist in current scope");
  return nullptr;
}

yalll::Function* Scope::find_function(const std::string& name) {
  logger->send_log("Searching for function {}", name);
  for (auto i = scope_frames.size() - 1; i >= 0; --i) {
    if (scope_frames.at(i).func_map.contains(name)) {
      logger->send_log("Function {} found", name);
      return &scope_frames.at(i).func_map.at(name);
    }
  }

  logger->send_error("Function with name {} does not exist", name);
  return nullptr;
}

void Scope::set_active_function(const std::string& name) {
  active_function = find_function(name);
  if (active_function)
    logger->send_log("Set active function to {}", name);
  else
    logger->send_internal_error("Failed to set active function");
}

void Scope::no_active_function() {
  active_function = nullptr;
  logger->send_log("Deactivated active function");
}

std::string& Scope::get_scope_ctx_name() {
  return scope_frames.back().ctx_name;
}
}  // namespace scoping
