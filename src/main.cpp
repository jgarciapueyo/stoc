#include <iostream>
#include <string>

#include <cxxopts.hpp>

#include "stoc/AST/ASTPrinter.h"
#include "stoc/Parser/Parser.h"
#include "stoc/Scanner/Scanner.h"
#include "stoc/SrcFile/SrcFile.h"

void initOptions(cxxopts::Options &options) {
  options.show_positional_help();

  options.add_options("basic")
      ("h,help", "Print help information")
      ("i,input", "Path to source code to compile",cxxopts::value<std::string>())
      ("o,output", "Output file",
        cxxopts::value<std::string>()->default_value("a.out"))
      ("s,scanning", "Show tokens after scannning",
        cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
      ("ast-dump", "Show AST after parsing",
        cxxopts::value<bool>()->default_value("false")->implicit_value("true"));

  options.parse_positional({"input", "output"});
}

int main(int argc, char *argv[]) {
  cxxopts::Options options(argv[0],"Compiler for stoc programming language");
  initOptions(options);
  auto opt = options.parse(argc, argv);

  if(opt.count("help")) {
    std::cout << options.help() <<  std::endl;
    return 0;
  }

  if(!opt.count("input")) {
    std::cerr << "Usage: " << options.get_program() << " <input file>" << std::endl;
    return 0;
  }

  try {
    // Read file
    std::string path = opt["input"].as<std::string>();
    auto src = std::make_shared<SrcFile>(path);

    // Scan file (lexing)
    Scanner scanner(src);
    scanner.scan();

    if(src->isErrorInScanning()) {
      return 1;
    }

    if(opt["scanning"].as<bool>()) {
      for(auto token : src->getTokens()) {
        std::cout << token << std::endl;
      }
    }

    // Parse file
    Parser parser(src);
    parser.parse();

    if(src->isErrorInParsing()) {
      return 1;
    }

    if(opt["ast-dump"].as<bool>()) {
      ASTPrinter printer;
      printer.print(src->getAst()[0]);
    }
  }
  catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}