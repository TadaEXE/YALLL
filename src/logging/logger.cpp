#include "logger.h"

namespace util {

void Logger::emit_msg() {
  auto msg = log_queue.front();

  switch (msg.type) {
    case LogType::Log:
      std::cout << create_indent(msg.depth) << "[LOG]" << msg.msg << std::endl;
      break;
    case LogType::Error:
      std::cerr << create_indent(msg.depth) << "\033[31;44m[ERR]" << msg.msg
                << "\033[0m" << std::endl;
      break;
    case LogType::Internal:
      std::cerr << create_indent(msg.depth) << "\033[33m[INT]" << msg.msg
                << "\033[0m" << std::endl;
      break;
    default:
      std::cout << "[UNKNOWN]" << msg.msg << std::endl;
  }
  last_depth = msg.depth;
  log_queue.pop();
}

void Logger::emit_all() {
  while (!log_queue.empty()) {
    emit_msg();
  }
}

std::string Logger::create_indent(uint32_t depth) {
  if (depth == 0) return "";

  std::string indent = "\u2514>";
  if (depth > 1) indent.insert(indent.begin(), depth - 1, '|');
  return std::move(indent);
}
}  // namespace util
