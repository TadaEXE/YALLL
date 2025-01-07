#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>

#include <fstream>
#include <iostream>

#include "YALLLLexer.h"
#include "YALLLParser.h"
#include "compiler/visitor_impl.h"

void load_test_prog() {
  const char* path = "../../programs/floats.y";
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

  yallc::YALLLVisitorImpl visitor;
  visitor.visit(ast);

  stream.close();
}

int main(int argc, char* argv[]) {
  load_test_prog();
  return 0;
}
