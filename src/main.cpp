//=== src/main.cpp - main entry point for the stoc compiler -------------------------*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file is the main entry point to the Stoc compiler. It reads the arguments and invokes the
// modules for the different phases.
//
//===------------------------------------------------------------------------------------------===//
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
      ("tokens-dump", "Show tokens after scannning",
        cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
      ("ast-dump", "Show AST after parsing",
      cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
      ("emit-llvm", "Show LLVM IR generated",
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
    // If some option of the compiler is activated like printing tokens, the AST or the LLVM IR, we
    // assume that the user does not want the executable
    bool wantsExecutable = true;

    // Scan file (lexing)
    Scanner scanner(src);
    scanner.scan();

    if (src->isErrorInScanning()) {
      return 1;
    }

    if (opt["tokens-dump"].as<bool>()) {
      scanner.printTokens();
      wantsExecutable = false;
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
      ASTPrinter printer;
      for (const auto &node : src->getAst()) {
        printer.print(node);
      }
      wantsExecutable = false;
    }

    // Code Generation
    CodeGeneration codegen(src);
    codegen.generate();

    if(opt["emit-llvm"].as<bool>()) {
      codegen.printLLVM();
      wantsExecutable = false;
    }

    if(src->isErrorInCodeGeneration()) {
        return 1;
    }

    if(wantsExecutable) {
      codegen.getExecutable();
    }

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}