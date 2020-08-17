//===- src/SemanticAnalysis/Type.cpp - Implementation of the Type class -------------*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements the Type class used in Semantic Analysis for type
// checking and in the symbols.
//
//===------------------------------------------------------------------------------------------===//

#include "stoc/SemanticAnalysis/Type.h"

// ---- Type ----
Type::Type(Type::Kind kind) : typeKind(kind) {}
Type::Kind Type::getTypeKind() { return typeKind; }

bool typeIsEqual(const std::shared_ptr<Type> lhs, const std::shared_ptr<Type> rhs) {
  if (lhs->typeKind == rhs->typeKind) {
    switch (lhs->typeKind) {
    case Type::Kind::BasicType:
      return typeIsEqual(std::dynamic_pointer_cast<BasicType>(lhs),
                         std::dynamic_pointer_cast<BasicType>(rhs));
    case Type::Kind::Signature:
      return typeIsEqual(std::dynamic_pointer_cast<FunctionType>(lhs),
                         std::dynamic_pointer_cast<FunctionType>(rhs));
    default:
      return false;
    }
  } else {
    return false;
  }
}

std::ostream &operator<<(std::ostream &os, const std::shared_ptr<Type> &type) {
  os << type->getName();
  return os;
}

// ---- Basic Type ----
BasicType::BasicType(Kind kind, std::string name)
    : Type(Type::Kind::BasicType), kind(kind), name(name) {}

std::shared_ptr<BasicType> BasicType::getBoolType() {
  return std::make_shared<BasicType>(BasicType::Kind::BOOL, "bool");
}

std::shared_ptr<BasicType> BasicType::getIntType() {
  return std::make_shared<BasicType>(BasicType::Kind::INT, "int");
}

std::shared_ptr<BasicType> BasicType::getFloatType() {
  return std::make_shared<BasicType>(BasicType::Kind::FLOAT, "float");
}

std::shared_ptr<BasicType> BasicType::getStringType() {
  return std::make_shared<BasicType>(BasicType::Kind::STRING, "string");
}

std::shared_ptr<BasicType> BasicType::getVoidType() {
  return std::make_shared<BasicType>(BasicType::Kind::VOID, "void");
}

std::shared_ptr<BasicType> BasicType::getInvalidType() {
  return std::make_shared<BasicType>(BasicType::Kind::INVALID, "invalid");
}

BasicType::Kind BasicType::getKind() { return kind; }
std::string BasicType::getName() { return name; }

bool BasicType::isNumeric() { return kind == Kind::INT || kind == Kind::FLOAT; }
bool BasicType::isString() { return kind == Kind::STRING; }
bool BasicType::isBoolean() { return kind == Kind::BOOL; }
bool BasicType::isInvalid() { return kind == Kind::INVALID; }
bool BasicType::isComparable() { return isNumeric() || isString() || isBoolean(); }
bool BasicType::isOrdered() { return isNumeric(); }

bool typeIsEqual(std::shared_ptr<BasicType> lhs, std::shared_ptr<BasicType> rhs) {
  return lhs->kind == rhs->kind;
}

// ---- Signature (for functions) ----
FunctionType::FunctionType(std::vector<std::shared_ptr<BasicType>> params,
                           std::shared_ptr<BasicType> result)
    : Type(Type::Kind::Signature), params(params), result(result) {}

std::vector<std::shared_ptr<BasicType>> FunctionType::getParams() { return params; }
std::shared_ptr<BasicType> FunctionType::getResult() { return result; }
std::string FunctionType::getName() {
  std::string name = "(";
  for (int i = 0; i < params.size(); ++i) {
    name += params[i]->getName();
    if (i != params.size() - 1) {
      name += ",";
    }
  }
  name += ") ";
  name += result->getName();
  return name;
}

bool FunctionType::isInvalid() {
  for (const auto &param : params) {
    if (param->isInvalid())
      return true;
  }

  return result->isInvalid();
}

bool areParametersEqual(std::vector<std::shared_ptr<BasicType>> lhs,
                        std::vector<std::shared_ptr<BasicType>> rhs) {
  // check arity of parameters
  if (lhs.size() != rhs.size()) {
    return false;
  }

  // check parameters
  for (int i = 0; i < lhs.size(); ++i) {
    if (!typeIsEqual(lhs[i], rhs[i])) {
      return false;
    }
  }

  return true;
}

bool typeIsEqual(std::shared_ptr<FunctionType> lhs, std::shared_ptr<FunctionType> rhs) {
  return areParametersEqual(lhs->getParams(), rhs->getParams()) &&
         typeIsEqual(lhs->result, rhs->result);
}
