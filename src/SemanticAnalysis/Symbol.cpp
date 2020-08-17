//===- src/SemanticAnalysis/Symbol.cpp - Implementaiton of Symbol class --*- C++ -*-===//
//
//===-------------------------------------------------------------------------------===//
//
// This file implements the Symbol class used in to store information about declarations
// in SymbolTable.
//
//===-------------------------------------------------------------------------------===//

#include "stoc/SemanticAnalysis/Symbol.h"

Symbol::Symbol(std::string identifier, Symbol::Kind kind, std::shared_ptr<Type> type,
               std::shared_ptr<Decl> declReference)
    : identifier(identifier), kind(kind), type(type), declReference(declReference) {}

Symbol::Symbol(std::string identifier, Symbol::Kind kind, std::shared_ptr<Type> type)
    : identifier(identifier), kind(kind), type(type) {}

const std::string &Symbol::getIdentifier() const { return identifier; }
Symbol::Kind Symbol::getKind() const { return kind; }
std::shared_ptr<Type> Symbol::getType() const { return type; }
const std::shared_ptr<Decl> &Symbol::getDeclReference() const { return declReference; }
