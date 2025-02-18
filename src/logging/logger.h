#pragma once

#include <cstdint>
#include <format>
#include <iostream>
#include <queue>
#include <source_location>
#include <string>
#include <string_view>

namespace util {

enum class LogType {
  Log,
  Error,
  Internal,
};

struct LogMessage {
  LogType type;
  std::string msg;
  uint32_t depth;
};

class Logger {
 public:
  template <typename... Args>
  void send_log(std::string_view fmt, Args&&... args) {
    LogMessage msg{LogType::Log,
                   std::vformat(fmt, std::make_format_args(args...)),
                   cur_depth};
    log_queue.push(msg);
    emit_msg();
  }

  template <typename... Args>
  void send_error(std::string_view fmt, Args&&... args) {
    LogMessage msg{LogType::Error,
                   std::vformat(fmt, std::make_format_args(args...)),
                   cur_depth};
    log_queue.push(msg);
    emit_msg();
  }

  template <typename... Args>
  void send_internal_error(std::string_view fmt, Args&&... args) {
    LogMessage msg{LogType::Internal,
                   std::vformat(fmt, std::make_format_args(args...)),
                   cur_depth};
    log_queue.push(msg);
    emit_msg();
  }

  void operator++() { cur_depth++; }
  void operator--() { cur_depth--; }
  void operator+=(uint32_t add) { cur_depth += add; }
  void operator-=(uint32_t sub) { cur_depth -= sub; }

  virtual void emit_msg();
  virtual void emit_all();

 protected:
  std::queue<LogMessage> log_queue;
  uint32_t cur_depth;

 private:
  uint32_t last_depth = 0;
  std::string create_indent(uint32_t depth);
};
}  // namespace util
