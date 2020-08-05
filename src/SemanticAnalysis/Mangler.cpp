//TODO: add header
#include "stoc/SemanticAnalysis/Mangler.h"

namespace mangler {

std::string mangle(std::string functionName, std::shared_ptr<FunctionType> functionType) {
  // Initial name
  std::string functionNameMangled = functionName;
  // If it is the main function we do not mangle it
  if(functionName == "main") {
    return functionNameMangled;
  }

  // Add start characters for parameters _(numberofparams)p_
  functionNameMangled += "_";
  functionNameMangled += std::to_string(functionType->getParams().size());
  functionNameMangled += "p_";

  // Type of every parameter
  for(const auto &param : functionType->getParams()) {
    functionNameMangled += param->getName();
  }

  // Add start characters for return type
  functionNameMangled += "_r";
  functionNameMangled += functionType->getResult()->getName();
  return functionNameMangled;
}

}

