// TODO: add header of the file
#include "stoc/SemanticAnalysis/SymbolTable.h"

SymbolTable::SymbolTable(int level) : level(level) {}

SymbolTable::SymbolTable(std::shared_ptr<SymbolTable> previousTable, int level)
    : previousTable(previousTable), level(level) {}

const std::shared_ptr<SymbolTable> &SymbolTable::getPreviousTable() const { return previousTable; }
int SymbolTable::getLevel() const { return level; }

Symbol SymbolTable::lookup(std::string identifier) {
  auto it = table.find(identifier);
  if(it == this->table.end()) { // it has not been found in current scope
    if(level == 0) { // if it is last scope, \identifier has not been found
      throw std::runtime_error("Undefined reference to " + identifier);
    }
    else { // look in previous scope/SymbolTable
      return this->previousTable->lookup(identifier);
    }
  }
  else {
    return it->second;
  }
}

void SymbolTable::insert(std::string identifier, Symbol symbol) {
  auto it = table.find(identifier);
  if(it == this->table.end()) { // it has not been found in current scope
    table.insert({identifier, symbol});
  }
  else {
    throw std::runtime_error("Redefinition of '" + identifier + "'");
  }
}
