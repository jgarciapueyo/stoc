//===- stoc/SemanticAnalysis/Symbol.cpp - Defintion of Symbol class ------*- C++ -*-===//
//
//===-------------------------------------------------------------------------------===//
//
// This file defines the Symbol class used in to store information about declarations
// in SymbolTable.
//
//===-------------------------------------------------------------------------------===//
#ifndef STOC_SYMBOL_H
#define STOC_SYMBOL_H

#include <vector>

#include "stoc/AST/Decl.h"
#include "stoc/SemanticAnalysis/Type.h"

/// Symbol that stores all the necessary information related to a symbol in the source code.
///  This information varies depending on the type of the symbol. For example:
///  For a variable, we may store the identifier and the type.
///  For a function, we may store the identifier, parameter types and return type.
///  All these are only examples, and more information may be stored.
class Symbol {
public:
  /// Types of symbols in source code
  enum class Kind { VARIABLE, CONSTANT, PARAMETER, FUNCTION };

private:
  /// string representation of the symbol. It is the name by which the symbol will be looked up
  std::string identifier;

  Kind kind;

  std::shared_ptr<Type> type;

  /// saves a reference to the declaration of this symbol. Used to bind declarations and usages
  /// of identifiers in expressions
  std::shared_ptr<Decl> declReference;

public:
  Symbol() = default;

  /// Constructor for variables, constant and parameters
  Symbol(std::string identifier, Symbol::Kind kind, std::shared_ptr<Type> type,
         std::shared_ptr<Decl> declReference);
  Symbol(std::string identifier, Symbol::Kind kind, std::shared_ptr<Type> type);

  // Getters
  [[nodiscard]] const std::string &getIdentifier() const;
  [[nodiscard]] Symbol::Kind getKind() const;
  [[nodiscard]] std::shared_ptr<Type> getType() const;
  [[nodiscard]] const std::shared_ptr<Decl> &getDeclReference() const;
};

#endif // STOC_SYMBOL_H
