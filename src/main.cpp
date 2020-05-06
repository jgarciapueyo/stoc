#include <iostream>
#include <string>

#include <cxxopts.hpp>

//#include <llvm/IR/Module.h>

#include "Scanner/Scanner.h"
#include "SrcFile/SrcFile.h"

void initOptions(cxxopts::Options &options) {
  options.show_positional_help();

  options.add_options("basic")
      ("h,help", "Print help information")
      ("i,input", "path to source code to compile",cxxopts::value<std::string>())
      ("o,output", "Output file",
        cxxopts::value<std::string>()->default_value("a.out"))
      ("s,scanning", "Show tokens after scannning",
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
    std::string path = opt["input"].as<std::string>();
    auto src = std::make_shared<SrcFile>(path);

    Scanner scanner(src);
    std::vector<Token> tokens = scanner.scan();

    if(opt["scanning"].as<bool>()) {
      for(auto token : tokens) {
        std::cout << token << std::endl;
      }
    }


  }
  catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}