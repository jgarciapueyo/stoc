// TODO: add header of the file
#ifndef STOC_SYMBOL_H
#define STOC_SYMBOL_H

#include "stoc/Scanner/Token.h"

#include <vector>

/// Symbol that stores all the necessary information related to a symbol in the source code.
///  This information varies depending on the type of the symbol. For example:
///  For a variable, we may store the identifier and the type.
///  For a function, we may store the identifier, parameter types and return type.
///  All these are only examples, and more information may be stored.
class Symbol {
public:
  /// Types of symbols in source code
  enum SymbolType {
    VARIABLE,
    CONSTANT,
    PARAMETER,
    FUNCTION
  };

private:
  /// string representation of the symbol. It is the name by which the symbol will be looked up
  std::string identifier;

  /// Type of the symbol
  SymbolType type;

  /// For VARIABLE/CONSTANT/PARAMETER: data type of the variable/constant/parameter (e.g INT, FLOAT, ...)
  /// For FUNCTION: return type of the function (e.g. INT, FLOAT, ...)
  std::string returnType; // type of a variable/constant/parameter or return type of a function

  // Token parameterType; if we can pass parameter const or var (right now it is not needed)
  // visible

  /// Only for FUNCTION: data type of the parameters in function
  std::vector<std::string> parameterListType;

public:
  Symbol() = default;

  /// Constructor for variables, constant and parameters
  Symbol(std::string identifier, SymbolType type, std::string returnType);

  /// Constructor for functions
  Symbol(std::string identifier, SymbolType type, std::string returnType, std::vector<std::string> parameterList);

  // Getters
  [[nodiscard]] const std::string &getIdentifier() const;
  [[nodiscard]] SymbolType getType() const;
  [[nodiscard]] const std::vector<std::string> &getParameterList() const;
  const std::string &getReturnType() const;
};

#endif // STOC_SYMBOL_H
