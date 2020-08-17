//===- src/SemanticAnalysis/SymbolTable.cpp - Implementation of SymbolTable class --*- C++ -*-===//
//
//===-----------------------------------------------------------------------------------------===//
//
// This file implements the SymbolTable class used in to store information about declarations
// (variables, constants and functions) and the usage of the identifiers later in the source
// program.
//
//===-----------------------------------------------------------------------------------------===//
#include "stoc/SemanticAnalysis/SymbolTable.h"
#include "stoc/SemanticAnalysis/Type.h"

SymbolTable::SymbolTable(int level) : level(level) {}

SymbolTable::SymbolTable(std::shared_ptr<SymbolTable> previousTable, int level)
    : previousTable(previousTable), level(level) {}

const std::shared_ptr<SymbolTable> &SymbolTable::getPreviousTable() const { return previousTable; }
int SymbolTable::getLevel() const { return level; }

std::vector<Symbol> SymbolTable::lookup(std::string identifier) {
  auto it = table.find(identifier);
  if (it == this->table.end()) { // it has not been found in current scope
    if (level == 0) {            // if it is last scope, \identifier has not been found
      throw std::runtime_error("Undefined reference to " + identifier);
    } else { // look in previous scope/SymbolTable
      return this->previousTable->lookup(identifier);
    }
  } else {
    return it->second;
  }
}

void SymbolTable::insertFunction(std::string identifier, Symbol symbol) {
  // functions are mangled, so more than one function can exist with the same identifier
  auto it = table.find(identifier);
  if (it == this->table.end()) { // it has not been found in current scope we can insert it
    table.insert({identifier, {symbol}});
  } else {
    // because we allow overloading of functions, maybe the identifier that exists is a function
    // with different type
    std::vector<Symbol> symbols = it->second;
    if (symbols[0].getKind() != Symbol::Kind::FUNCTION) {
      // symbol in symbol table is a variable/constant/parameter so we can not insert the function
      throw std::runtime_error("Redefinition of '" + identifier + "'");
    }

    // symbol identifier in symbolTable is a function. We can insert the new function if parameter
    // types of all functions are different (overload of functions)
    for (const auto &functionSymbol : symbols) {
      if (areParametersEqual(
              std::dynamic_pointer_cast<FunctionType>(functionSymbol.getType())->getParams(),
              std::dynamic_pointer_cast<FunctionType>(symbol.getType())->getParams())) {
        throw std::runtime_error("Redefinition of '" + identifier + "'");
      }
    }

    // If all functions with same identifier have different parameter types, we insert the new one
    it->second.push_back(symbol);
  }
}

void SymbolTable::insertVariable(std::string identifier, Symbol symbol) {
  // variables, constants and parameters are not mangled, so only one variable with the same
  // identifier can exist
  auto it = table.find(identifier);
  if (it == this->table.end()) { // it has not been found in current scope we can insert it
    table.insert({identifier, {symbol}});
  } else { // we can not insert it
    throw std::runtime_error("Redefinition of '" + identifier + "'");
  }
}

void SymbolTable::insert(std::string identifier, Symbol symbol) {
  if (symbol.getKind() == Symbol::Kind::FUNCTION) {
    insertFunction(identifier, symbol);
  } else { // is VARIABLE/CONSTANT/PARAMETER
    insertVariable(identifier, symbol);
  }
}
