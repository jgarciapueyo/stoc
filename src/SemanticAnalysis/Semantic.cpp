//===- src/SemanticAnalysis/Semantic.cpp - Implementation of Semantic class ---------*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements the Semantic class.
// Semantic Analysis is the third phase of a compiler and computes additional information after the
// syntactic structure of the program is known. t is called semantic analysisbecause the information
// goes beyond what a context-free grammar can express andit is closer related with the meaning of
// the program. In a statically typed languagelike Stoc, semantic analysis consists on building a
// symbol table to connect variables definition and their use, and also doing type checking.
//
//===------------------------------------------------------------------------------------------===//
#include "stoc/SemanticAnalysis/Semantic.h"

#include <unordered_map>

#include "stoc/AST/Decl.h"
#include "stoc/AST/Expr.h"
#include "stoc/AST/Stmt.h"
#include "stoc/SemanticAnalysis/Mangler.h"
#include "stoc/SemanticAnalysis/Type.h"

Semantic::Semantic(std::shared_ptr<SrcFile> file)
    : file(file), scopeLevel(0), scopeType(ScopeType::NONE) {
  this->symbolTable = std::make_shared<SymbolTable>(0);

  returnStatementInBlockStmt = false;
  declareBuiltinFunctions();
}

void Semantic::analyse() {
  for (const auto &declaration : file->getAst()) {
    declaration->accept(this);
  }

  // Check in the end that there is at least one main function
  std::vector<Symbol> symbols = symbolTable->lookup("main");
  if (symbols.empty()) {
    reportError("missing main function");
  }
}

void Semantic::analyse(const std::shared_ptr<Expr> &expr) { expr->accept(this); }

void Semantic::analyse(const std::shared_ptr<Stmt> &stmt) { stmt->accept(this); }

void Semantic::analyse(const std::shared_ptr<Decl> &decl) { decl->accept(this); }

void Semantic::analyse(const std::vector<std::shared_ptr<Stmt>> &stmts) {
  bool previousReturnStatementInBlockStmt = returnStatementInBlockStmt;
  returnStatementInBlockStmt = false;
  for (const auto &stmt : stmts) {
    if (returnStatementInBlockStmt) {
      reportError("Statement after return statement");
    }
    analyse(stmt);
  }
  returnStatementInBlockStmt = previousReturnStatementInBlockStmt;
}

void Semantic::declareBuiltinFunctions() {
  auto type_void = BasicType::getVoidType();
  // print function for basic types
  std::vector<std::shared_ptr<BasicType>> params_print_int{BasicType::getIntType()};
  auto type_print_int = std::make_shared<FunctionType>(params_print_int, type_void);
  Symbol symbol_print_int("print", Symbol::Kind::FUNCTION, type_print_int);
  symbolTable->insert("print", symbol_print_int);

  std::vector<std::shared_ptr<BasicType>> params_print_float{BasicType::getFloatType()};
  auto type_print_float = std::make_shared<FunctionType>(params_print_float, type_void);
  Symbol symbol_print_float("print", Symbol::Kind::FUNCTION, type_print_float);
  symbolTable->insert("print", symbol_print_float);

  std::vector<std::shared_ptr<BasicType>> params_print_bool{BasicType::getBoolType()};
  auto type_print_bool = std::make_shared<FunctionType>(params_print_bool, type_void);
  Symbol symbol_print_bool("print", Symbol::Kind::FUNCTION, type_print_bool);
  symbolTable->insert("print", symbol_print_bool);

  std::vector<std::shared_ptr<BasicType>> params_print_string{BasicType::getStringType()};
  auto type_print_string = std::make_shared<FunctionType>(params_print_string, type_void);
  Symbol symbol_print_string("print", Symbol::Kind::FUNCTION, type_print_string);
  symbolTable->insert("print", symbol_print_string);

  // println function for basic types
  std::vector<std::shared_ptr<BasicType>> params_println_int{BasicType::getIntType()};
  auto type_println_int = std::make_shared<FunctionType>(params_println_int, type_void);
  Symbol symbol_println_int("println", Symbol::Kind::FUNCTION, type_println_int);
  symbolTable->insert("println", symbol_println_int);

  std::vector<std::shared_ptr<BasicType>> params_println_float{BasicType::getFloatType()};
  auto type_println_float = std::make_shared<FunctionType>(params_println_float, type_void);
  Symbol symbol_println_float("println", Symbol::Kind::FUNCTION, type_println_float);
  symbolTable->insert("println", symbol_println_float);

  std::vector<std::shared_ptr<BasicType>> params_println_bool{BasicType::getBoolType()};
  auto type_println_bool = std::make_shared<FunctionType>(params_println_bool, type_void);
  Symbol symbol_println_bool("println", Symbol::Kind::FUNCTION, type_println_bool);
  symbolTable->insert("println", symbol_println_bool);

  std::vector<std::shared_ptr<BasicType>> params_println_string{BasicType::getStringType()};
  auto type_println_string = std::make_shared<FunctionType>(params_println_string, type_void);
  Symbol symbol_println_string("println", Symbol::Kind::FUNCTION, type_println_string);
  symbolTable->insert("println", symbol_println_string);
}

void Semantic::beginScope() {
  scopeLevel++;
  std::shared_ptr<SymbolTable> currentSymbolTable =
      std::make_shared<SymbolTable>(symbolTable, scopeLevel);
  this->symbolTable = currentSymbolTable;
}

void Semantic::endScope() {
  this->symbolTable = this->symbolTable->getPreviousTable();
  scopeLevel--;
}

void Semantic::reportError(std::string error_msg, int line, int column) {
  std::cerr << "<" << this->file->getFilename() << ":l" << line << ":c" << column
            << "> Semantic analysis error: " << error_msg << std::endl;

  this->file->setErrorInSemanticAnalysis(true);
}

void Semantic::reportError(std::string error_msg) {
  std::cerr << "<" << this->file->getFilename() << "> Semantic analysis error: " << error_msg
            << std::endl;

  this->file->setErrorInSemanticAnalysis(true);
}

bool isNumeric(std::shared_ptr<Type> type) {
  switch (type->getTypeKind()) {
  case Type::Kind::BasicType:
    return std::dynamic_pointer_cast<BasicType>(type)->isNumeric();
  case Type::Kind::Signature:
    return false;
  default:
    return false;
  }
}

bool isString(std::shared_ptr<Type> type) {
  switch (type->getTypeKind()) {
  case Type::Kind::BasicType:
    return std::dynamic_pointer_cast<BasicType>(type)->isString();
  case Type::Kind::Signature:
    return false;
  default:
    return false;
  }
}

bool isBoolean(std::shared_ptr<Type> type) {
  switch (type->getTypeKind()) {
  case Type::Kind::BasicType:
    return std::dynamic_pointer_cast<BasicType>(type)->isBoolean();
  case Type::Kind::Signature:
    return false;
  default:
    return false;
  }
}

bool isComparable(std::shared_ptr<Type> type) {
  switch (type->getTypeKind()) {
  case Type::Kind::BasicType:
    return std::dynamic_pointer_cast<BasicType>(type)->isComparable();
  case Type::Kind::Signature:
    return false;
  default:
    return false;
  }
}

bool isOrdered(std::shared_ptr<Type> type) {
  switch (type->getTypeKind()) {
  case Type::Kind::BasicType:
    return std::dynamic_pointer_cast<BasicType>(type)->isOrdered();
  case Type::Kind::Signature:
    return false;
  default:
    return false;
  }
}

bool isComparator(TokenType tokenType) {
  switch (tokenType) {
  case EQUAL:
  case NOT_EQUAL:
  case LESS:
  case GREATER:
  case LESS_EQUAL:
  case GREATER_EQUAL:
    return true;
  default:
    return false;
  }
}

bool passRequirements(std::vector<std::function<bool(std::shared_ptr<Type>)>> requirements,
                      std::shared_ptr<Type> type) {
  for (const auto &requirement : requirements) {
    if (!requirement(type)) {
      return false;
    }
  }
  return true;
}

std::pair<bool, std::shared_ptr<Type>>
Semantic::isValidBinaryOperatorForType(const Token &op, std::shared_ptr<Type> typeOperands) {
  static std::unordered_map<TokenType, std::vector<std::function<bool(std::shared_ptr<Type>)>>>
      binaryOp = {
          // I was not able to implement concatenation with strings in code generation
          //{ADD, {[](std::shared_ptr<Type> t) { return isNumeric(t) || isString(t); }}},
          {ADD, {isNumeric}},           {SUB, {isNumeric}},      {STAR, {isNumeric}},
          {SLASH, {isNumeric}},         {EQUAL, {isComparable}}, {NOT_EQUAL, {isComparable}},
          {LESS, {isOrdered}},          {GREATER, {isOrdered}},  {LESS_EQUAL, {isOrdered}},
          {GREATER_EQUAL, {isOrdered}}, {LAND, {isBoolean}},     {LOR, {isBoolean}},
      };

  auto requirements = binaryOp.find(op.tokenType);
  if (requirements != binaryOp.end() && passRequirements(requirements->second, typeOperands)) {
    return {true, isComparator(op.tokenType) ? BasicType::getBoolType() : typeOperands};
  } else {
    return {false, nullptr};
  }
}

std::pair<bool, std::shared_ptr<Type>>
Semantic::isValidUnaryOperatorForType(const Token &op, std::shared_ptr<Type> typeOperands) {
  static std::unordered_map<TokenType, std::vector<std::function<bool(std::shared_ptr<Type>)>>>
      unaryOp = {{ADD, {isNumeric}}, {SUB, {isNumeric}}, {NOT, {isBoolean}}};

  auto requirements = unaryOp.find(op.tokenType);
  if (requirements != unaryOp.end() && passRequirements(requirements->second, typeOperands)) {
    return {true, isComparator(op.tokenType) ? BasicType::getBoolType() : typeOperands};
  } else {
    return {false, nullptr};
  }
}

std::shared_ptr<Type> Semantic::tokenTypeToType(Token token) {
  switch (token.tokenType) {
  case LIT_TRUE:
  case LIT_FALSE:
  case BOOL:
    return BasicType::getBoolType();
  case LIT_INT:
  case INT:
    return BasicType::getIntType();
  case LIT_FLOAT:
  case FLOAT:
    return BasicType::getFloatType();
  case LIT_STRING:
  case STRING:
    return BasicType::getStringType();
  default:
    reportError("Internal Error - Token type not recognized", token.line, token.line);
    return BasicType::getInvalidType();
  }
}

void Semantic::visit(std::shared_ptr<VarDecl> node) {
  // Do semantic analysis of initializer expression (needed to calculate type)
  analyse(node->getValue());

  // Type checking
  node->setType(tokenTypeToType(node->getTypeToken()));

  // if node->getValue() is invalid, the error during initialization has already been reported and
  // we do not have to do nothing.
  if (!node->getValue()->getType()->isInvalid()) {
    if (!typeIsEqual(node->getType(), node->getValue()->getType())) {
      reportError("Type checking: different types " + node->getType()->getName() + " and " +
                      node->getValue()->getType()->getName(),
                  node->getTypeToken().line, node->getTypeToken().column);
    }
  }

  // Update information about if constant declaration is global
  bool isGlobal = this->scopeLevel == 0;
  node->setIsGlobal(isGlobal);

  // Update symbol table with new variable
  Symbol symbol(node->getIdentifierToken().value, Symbol::Kind::VARIABLE, node->getType(), node);
  try {
    symbolTable->insert(symbol.getIdentifier(), symbol);
  } catch (std::runtime_error &e) {
    reportError(e.what(), node->getIdentifierToken().line, node->getIdentifierToken().column);
  }
}

void Semantic::visit(std::shared_ptr<ConstDecl> node) {
  // Do semantic analysis of initializer expression (needed to calculate type)
  analyse(node->getValue());

  // Type checking
  node->setType(tokenTypeToType(node->getTypeToken()));

  // if node->getValue() is invalid, the error during initialization has been already reported and
  // we do not have to do nothing.
  if (!node->getValue()->getType()->isInvalid()) {
    if (!typeIsEqual(node->getType(), node->getValue()->getType())) {
      reportError("Type checking: different types " + node->getType()->getName() + " and " +
                      node->getValue()->getType()->getName(),
                  node->getTypeToken().line, node->getTypeToken().column);
    }
  }

  // Update information about if constant declaration is global
  bool isGlobal = this->scopeLevel == 0;
  node->setIsGlobal(isGlobal);

  // Update symbol table with new constant
  Symbol symbol(node->getIdentifierToken().value, Symbol::Kind::CONSTANT, node->getType(), node);
  try {
    symbolTable->insert(symbol.getIdentifier(), symbol);
  } catch (std::runtime_error &e) {
    reportError(e.what(), node->getIdentifierToken().line, node->getIdentifierToken().column);
  }
}

void Semantic::visit(std::shared_ptr<ParamDecl> node) {
  // Type checking
  node->setType(tokenTypeToType(node->getTypeToken()));

  // Update symbol table with new parameter
  Symbol symbol(node->getIdentifierToken().value, Symbol::Kind::PARAMETER, node->getType(), node);
  try {
    symbolTable->insert(symbol.getIdentifier(), symbol);
  } catch (std::runtime_error &e) {
    reportError(e.what(), node->getIdentifierToken().line, node->getIdentifierToken().column);
  }
}

std::shared_ptr<FunctionType> Semantic::createSignature(std::shared_ptr<FuncDecl> node) {
  std::vector<std::shared_ptr<BasicType>> params;

  for (const auto &parameter : node->getParams()) {
    params.push_back(
        std::dynamic_pointer_cast<BasicType>(tokenTypeToType(parameter->getTypeToken())));
  }

  std::shared_ptr<BasicType> returnType = nullptr;
  if (node->isHasReturnType()) {
    returnType = std::dynamic_pointer_cast<BasicType>(tokenTypeToType(node->getReturnTypeToken()));
  } else {
    returnType = BasicType::getVoidType();
  }
  return std::make_shared<FunctionType>(params, returnType);
}

void Semantic::visit(std::shared_ptr<FuncDecl> node) {
  auto prevScopeType = scopeType;
  scopeType = Semantic::ScopeType::FUNCTION;
  signature = createSignature(node);

  // Insert function identifier in scope
  Symbol symbol(node->getIdentifierToken().value, Symbol::Kind::FUNCTION, signature, node);
  try {
    symbolTable->insert(symbol.getIdentifier(), symbol);
  } catch (std::runtime_error &e) {
    reportError(e.what(), node->getIdentifierToken().line, node->getIdentifierToken().column);
  }
  node->setType(signature);

  // Analyse parameters and body of the function
  beginScope();
  for (const auto &parameter : node->getParams()) {
    analyse(parameter);
  }

  // In FuncDecl, declarations of parameters have to be inside the scope of the body so the scope
  //  it is created in the beginning. Because the scope has been already created, we call directly
  //  to analyse the vector of statements since analyse block of statements creates a new scope
  //  and we do not want a new scope
  analyse(node->getBody()->getStmts());
  endScope();

  // Restore previous values
  scopeType = prevScopeType;

  // Mangle the identifier of the function
  node->setIdentifierMangled(mangler::mangle(node->getIdentifierToken().value, signature));
}

void Semantic::visit(std::shared_ptr<DeclarationStmt> node) { analyse(node->getDecl()); }

void Semantic::visit(std::shared_ptr<ExpressionStmt> node) { analyse(node->getExpr()); }

void Semantic::visit(std::shared_ptr<BlockStmt> node) {
  beginScope();

  bool previousReturnStatementInBlockStmt = returnStatementInBlockStmt;
  returnStatementInBlockStmt = false;
  for (const auto &stmt : node->getStmts()) {
    if (returnStatementInBlockStmt) {
      reportError("In this block, there is a statement after return statement",
                  node->getLbrace().line, node->getRbrace().column);
    }
    analyse(stmt);
  }
  returnStatementInBlockStmt = previousReturnStatementInBlockStmt;

  endScope();
}

void Semantic::visit(std::shared_ptr<IfStmt> node) {
  analyse(node->getCondition());

  // Type checking for condition
  auto t = std::dynamic_pointer_cast<BasicType>(node->getCondition()->getType());
  if (t == nullptr || !t->isBoolean()) {
    reportError("Type checking: type of condition in if statement should be 'bool' but "
                "found " +
                    node->getCondition()->getType()->getName(),
                node->getIfKeyword().line, node->getIfKeyword().column);
  }

  analyse(node->getThenBranch());

  if (node->isHasElse()) {
    analyse(node->getElseBranch());
  }
}

void Semantic::visit(std::shared_ptr<ForStmt> node) {
  beginScope();
  analyse(node->getInit());
  analyse(node->getCond());

  // Type checking for condition
  auto t = std::dynamic_pointer_cast<BasicType>(node->getCond()->getType());
  if (t == nullptr || !t->isBoolean()) {
    reportError("Type checking: type of condition in for statement should be 'bool' but "
                "found " +
                    node->getCond()->getType()->getName(),
                node->getForKeyword().line, node->getForKeyword().column);
  }

  analyse(node->getPost());
  // In ForStmt, declarations in init have to be inside the scope of the body so the scope it is
  //  created in the beginning. Because the scope has been already created, we call directly to
  //  analyse the vector of statements since analyse block of statements creates a new scope
  //  and we do not want a new scope
  analyse(node->getBody()->getStmts());
  endScope();
}

void Semantic::visit(std::shared_ptr<WhileStmt> node) {
  analyse(node->getCond());

  // Type checking for condition
  auto t = std::dynamic_pointer_cast<BasicType>(node->getCond()->getType());
  if (t == nullptr || !t->isBoolean()) {
    reportError("Type checking: type of condition in while statement should be 'bool' but "
                "found " +
                    node->getCond()->getType()->getName(),
                node->getWhileKeyword().line, node->getWhileKeyword().column);
  }

  beginScope();
  analyse(node->getBody());
  endScope();
}

void Semantic::visit(std::shared_ptr<AssignmentStmt> node) {
  // check that it is assignable
  analyse(node->getLhs());
  analyse(node->getRhs());

  // Expression on the left hand side can be assigned
  if (node->getLhs()->getExprValueKind() == Expr::ValueKind::RVal) {
    reportError("Expression is not assignable", node->getEqualToken().line,
                node->getEqualToken().column);
  } else if (node->getLhs()->getExprValueKind() == Expr::ValueKind::NMod_LVal) {
    reportError("Expression is not assignable (constant)", node->getEqualToken().line,
                node->getEqualToken().column);
  }

  // Type checking
  if (!typeIsEqual(node->getRhs()->getType(), node->getLhs()->getType())) {
    reportError("Type checking: cannot assign type " + node->getRhs()->getType()->getName() +
                    " to type " + node->getLhs()->getType()->getName(),
                node->getEqualToken().line, node->getEqualToken().column);
  }
}

void Semantic::visit(std::shared_ptr<ReturnStmt> node) {
  if (scopeType != Semantic::ScopeType::FUNCTION) {
    reportError("return statement outside function body", node->getReturnKeyword().line,
                node->getReturnKeyword().column);
  } else {
    analyse(node->getValue());

    // Type checking
    if (!typeIsEqual(node->getValue()->getType(), this->signature->getResult())) {
      reportError("Type checking: different types of returned value of type " +
                      node->getValue()->getType()->getName() +
                      " and function return value of type " + this->signature->getName(),
                  node->getReturnKeyword().line, node->getReturnKeyword().column);
    }

    returnStatementInBlockStmt = true;
  }
}

void Semantic::visit(std::shared_ptr<BinaryExpr> node) {
  // Do semantic analysis of both expressions
  analyse(node->getLhs());
  analyse(node->getRhs());

  // Type checking if non of the operators are invalid. If any of the operators is invalid, the
  // error has already been reported and we do nothing.
  if (node->getLhs()->getType()->isInvalid() || node->getRhs()->getType()->isInvalid()) {
    node->setType(BasicType::getInvalidType());
    return;
  }

  // Type checking
  if (!typeIsEqual(node->getLhs()->getType(), node->getRhs()->getType())) {
    reportError("Type checking: different types " + node->getLhs()->getType()->getName() + " and " +
                    node->getRhs()->getType()->getName(),
                node->getOp().line, node->getOp().column);
  }

  // Type checking that operator is valid for types
  auto [isValid, type] = isValidBinaryOperatorForType(node->getOp(), node->getRhs()->getType());

  if (!isValid) {
    reportError("Operator is not supported for binary expression of type " +
                    node->getRhs()->getType()->getName(),
                node->getOp().line, node->getOp().column);
  }
  node->setType(type);
  node->setExprValueKind(Expr::ValueKind::RVal);
}

void Semantic::visit(std::shared_ptr<UnaryExpr> node) {
  // Do semantic analysis of expression
  analyse(node->getRhs());

  // Type checking if the operator is invalid. If  is invalid, the error has already been reported
  // and we do nothing.
  if (node->getRhs()->getType()->isInvalid()) {
    node->setType(BasicType::getInvalidType());
    return;
  }

  // Type checking that operator is valid for type
  auto [isValid, type] = isValidUnaryOperatorForType(node->getOp(), node->getRhs()->getType());

  if (!isValid) {
    reportError("Operator is not supported for unary expression of type " +
                    node->getRhs()->getType()->getName(),
                node->getOp().line, node->getOp().column);
  }
  node->setType(type);
  // Right now, all unary operators create a RValue. If we had arrays [] or address operators &,
  // we would have to check the current ExprValueKind and what it results from applying the unary op
  node->setExprValueKind(Expr::ValueKind::RVal);
}

void Semantic::visit(std::shared_ptr<LiteralExpr> node) {
  // Type checking
  node->setType(tokenTypeToType(node->getToken()));
  node->setExprValueKind(Expr::ValueKind::RVal);
}

void Semantic::visit(std::shared_ptr<IdentExpr> node) {
  try {
    std::vector<Symbol> symbols = symbolTable->lookup(node->getName());

    // if any of the symbol is a function we do nothing since we have already resolved the symbol
    // and now is the CallExpr node who will processed that
    if (symbols.empty()) {
      reportError("Internal Error - Identifier found but not symbol associated",
                  node->getIdent().line, node->getIdent().column);
    }

    if (symbols[0].getKind() == Symbol::Kind::FUNCTION) {
      resolvedSymbols = symbols;
      return;
    }

    if (symbols.size() > 1) {
      reportError("Internal Error - Multiple identifier of kind "
                  "variable/constant/parameter",
                  node->getIdent().line, node->getIdent().column);
    }

    // is a variable/constant/parameter
    node->setType(symbols[0].getType());

    // If identifer is a constant, it can not be modified
    if (symbols[0].getKind() == Symbol::Kind::CONSTANT) {
      node->setExprValueKind(Expr::ValueKind::NMod_LVal);
    } else {
      node->setExprValueKind(Expr::ValueKind::Mod_LVal);
    }

    node->setDeclOfIdentifier(symbols[0].getDeclReference());
  } catch (std::runtime_error &e) {
    reportError(e.what(), node->getIdent().line, node->getIdent().column);
    node->setType(BasicType::getInvalidType());
  }
}

void Semantic::visit(std::shared_ptr<CallExpr> node) {
  analyse(node->getFunc());
  // Save the identifier symbols of our function
  auto previousResolvedSymbols = resolvedSymbols;

  for (const auto &argument : node->getArgs()) {
    analyse(argument);
  }

  // Restore previous resolved symbols
  resolvedSymbols = previousResolvedSymbols;

  // Construct type of our call (basic type because for now we only have this)
  std::vector<std::shared_ptr<BasicType>> args;
  for (const auto &arg : node->getArgs()) {
    args.push_back(std::dynamic_pointer_cast<BasicType>(arg->getType()));
  }

  Symbol resolvedSymbol;
  bool foundSymbol = false;
  for (auto const &possibleSymbol : resolvedSymbols) {
    if (areParametersEqual(
            args, std::dynamic_pointer_cast<FunctionType>(possibleSymbol.getType())->getParams())) {
      resolvedSymbol = possibleSymbol;
      foundSymbol = true;
    }
  }

  if (foundSymbol) {
    auto functionType = std::dynamic_pointer_cast<FunctionType>(resolvedSymbol.getType());
    node->setType(functionType->getResult());
    node->getFunc()->setType(resolvedSymbol.getType());
    std::dynamic_pointer_cast<IdentExpr>(node->getFunc())
        ->setDeclOfIdentifier(resolvedSymbol.getDeclReference());
  } else {
    reportError("Undefined reference to " + resolvedSymbols[0].getIdentifier(),
                std::dynamic_pointer_cast<IdentExpr>(node->getFunc())->getIdent().line,
                std::dynamic_pointer_cast<IdentExpr>(node->getFunc())->getIdent().column);
    node->setType(BasicType::getInvalidType());
  }
}