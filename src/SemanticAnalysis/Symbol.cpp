// TODO: add header of the file
#include "stoc/SemanticAnalysis/Symbol.h"

Symbol::Symbol(std::string identifier, SymbolType type, std::string returnType)
    : identifier(identifier), type(type), returnType(returnType) {}

Symbol::Symbol(std::string identifier, SymbolType type, std::string returnType,
               std::vector<std::string> parameterListType)
    : identifier(identifier), type(type), returnType(returnType),
      parameterListType(parameterListType) {}

const std::string &Symbol::getIdentifier() const { return identifier; }
Symbol::SymbolType Symbol::getType() const { return type; }
const std::vector<std::string> &Symbol::getParameterList() const { return parameterListType; }
const std::string &Symbol::getReturnType() const { return returnType; }
