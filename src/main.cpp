#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include <fstream>
#include <iostream>

#include "YALLLLexer.h"
#include "YALLLParser.h"
#include "compiler/visitor_impl.h"

void load_test_prog(const char *arg_path = nullptr,
                    const char *arg_out_path = nullptr) {
  const char *path;
  const char *out_path;
  if (arg_path)
    path = arg_path;
  else
    path = "../programs/floats.y";

  if (arg_out_path)
    out_path = arg_out_path;
  else
    out_path = "../output.ll";

  std::cout << "Loading: " << path << std::endl;

  std::ifstream stream(path);
  if (!stream.good()) {
    std::cout << "Bad path, aborting!" << std::endl;
    return;
  }

  antlr4::ANTLRInputStream input(stream);
  YALLLLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  YALLLParser parser(&tokens);

  auto ast = parser.program();
  std::cout << ast->getText() << std::endl;

  yallc::YALLLVisitorImpl visitor(out_path);
  visitor.visit(ast);

  stream.close();
}

char *get_cmd_option(char **begin, char **end, const std::string &option) {
  char **itr = std::find(begin, end, option);
  if (itr != end && ++itr != end) {
    return *itr;
  }
  return nullptr;
}

bool cmd_option_exists(char **begin, char **end, const std::string &option) {
  return std::find(begin, end, option) != end;
}

int main(int argc, char *argv[]) {
  char *arg_file;
  char *arg_out_path;
  if (cmd_option_exists(argv, argv + argc, "-f")) {
    arg_file = get_cmd_option(argv, argv + argc, "-f");
  }

  if (cmd_option_exists(argv, argv + argc, "-o")) {
    arg_out_path = get_cmd_option(argv, argv + argc, "-o");
  }

  load_test_prog(arg_file, arg_out_path);
  return 0;
}
