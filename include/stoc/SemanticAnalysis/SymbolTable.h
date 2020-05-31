// TODO: add header of the file
#ifndef STOC_SYMBOLTABLE_H
#define STOC_SYMBOLTABLE_H

#include "Symbol.h"
#include "stoc/Scanner/Token.h"

#include <memory>
#include <string>
#include <unordered_map>

/// Structure used to store information associated to the symbols declared in the program
/// <identifier, [information]>
class SymbolTable {
private:
  /// SymbolTable of the previous scope
  std::shared_ptr<SymbolTable> previousTable;

  /// Table of the current scope
  std::unordered_map<std::string, Symbol> table;

  /// Depth level of the scope associated with this SymbolTable
  int level;

public:
  explicit SymbolTable(int level);

  SymbolTable(std::shared_ptr<SymbolTable> previousTable, int level);

  // Getters
  const std::shared_ptr<SymbolTable> &getPreviousTable() const;
  int getLevel() const;

  /// Looks \identifier up in the current SymbolTable and, if not found, looks up in previous table.
  ///  If it is found, it returns the Symbol associated with the identifier.
  ///  If \identifier is not found in any SymbolTable, it raises an exception.
  Symbol lookup(std::string identifier);

  /// It inserts \symbol associated with \identifier in the current scope/SymbolTable.
  ///  If there is already a \symbol associated with \identifier in the current scope/SymbolTable
  ///  it raises an exception.
  void insert(std::string identifier, Symbol symbol);

  // delete (but why, for deleting only necessary to go to a symbol table with more level
};
#endif // STOC_SYMBOLTABLE_H
