// TODO: add header of the file
#include <iostream>
#include <string>

#include <cxxopts.hpp>

#include "stoc/AST/ASTPrinter.h"
#include "stoc/CodeGeneration/CodeGeneration.h"
#include "stoc/Parser/Parser.h"
#include "stoc/Scanner/Scanner.h"
#include "stoc/SemanticAnalysis/Semantic.h"
#include "stoc/SrcFile/SrcFile.h"

void initOptions(cxxopts::Options &options) {
  options.show_positional_help();

  options.add_options("basic")
      ("h,help", "Print help information")
      ("i,input", "Path to source code to compile", cxxopts::value<std::string>())
      ("o,output", "Output file", cxxopts::value<std::string>()->default_value("a.out"))
      ("s,scanning", "Show tokens after scannning",
        cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
      ("ast-dump", "Show AST after parsing",
      cxxopts::value<bool>()->default_value("false")->implicit_value("true"));

  options.parse_positional({"input", "output"});
}

int main(int argc, char *argv[]) {
  cxxopts::Options options(argv[0], "Compiler for stoc programming language");
  initOptions(options);
  auto opt = options.parse(argc, argv);

  if (opt.count("help")) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  if (!opt.count("input")) {
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

    if (src->isErrorInScanning()) {
      return 1;
    }

    if (opt["scanning"].as<bool>()) {
      scanner.printTokens();
    }

    // Parse file (parsing)
    Parser parser(src);
    parser.parse();

    if (src->isErrorInParsing()) {
      return 1;
    }

    // Semantic Analysis
    Semantic semantic(src);
    semantic.analyse();

    if (src->isErrorInSemanticAnalysis()) {
      return 1;
    }

    if (opt["ast-dump"].as<bool>()) {
      // TODO: remove and call from print()
      ASTPrinter printer;
      for (const auto &node : src->getAst()) {
        printer.print(node);
      }
    }

    // Code Generation
    CodeGeneration codegen(src);
    codegen.generate();

    // LLVM IR to executable

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}