//===- stoc/SemanticAnalysis/SymbolTable.h - Defintion of SymbolTable class --------*- C++ -*-===//
//
//===-----------------------------------------------------------------------------------------===//
//
// This file defines the SymbolTable class used in to store information about declarations
// (variables, constants and functions) and the usage of the identifiers later in the source
// program.
//
//===-----------------------------------------------------------------------------------------===//
#ifndef STOC_SYMBOLTABLE_H
#define STOC_SYMBOLTABLE_H

#include <memory>
#include <string>
#include <unordered_map>

#include "stoc/Scanner/Token.h"
#include "stoc/SemanticAnalysis/Symbol.h"

/// Structure used to store information associated to the symbols declared in the program
/// <identifier, [information]>
class SymbolTable {
private:
  /// SymbolTable of the previous scope
  std::shared_ptr<SymbolTable> previousTable;

  /// Table of the current scope
  std::unordered_map<std::string, std::vector<Symbol>> table;

  /// Depth level of the scope associated with this SymbolTable
  int level;

  /// use to insert function
  void insertFunction(std::string identifier, Symbol symbol);

  /// use to insert variables/constants/parameters
  void insertVariable(std::string identifier, Symbol symbol);

public:
  explicit SymbolTable(int level);

  SymbolTable(std::shared_ptr<SymbolTable> previousTable, int level);

  // Getters
  const std::shared_ptr<SymbolTable> &getPreviousTable() const;
  int getLevel() const;

  /// Looks \identifier up in the current SymbolTable and, if not found, looks up in previous table.
  ///  If it is found, it returns the Symbol associated with the identifier.
  ///  If \identifier is not found in any SymbolTable, it raises an exception.
  std::vector<Symbol> lookup(std::string identifier);

  /// It inserts \symbol associated with \identifier in the current scope/SymbolTable.
  ///  If there is already a \symbol associated with \identifier in the current scope/SymbolTable
  ///  it raises an exception.
  void insert(std::string identifier, Symbol symbol);
};
#endif // STOC_SYMBOLTABLE_H
