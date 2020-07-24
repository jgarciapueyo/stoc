// TODO: add header of the file
// TODO: semantic analysis has to transform AST to a better suited AST for code Generation
#include "stoc/SemanticAnalysis/Semantic.h"

#include "stoc/AST/Decl.h"
#include "stoc/AST/Expr.h"
#include "stoc/AST/Stmt.h"

Semantic::Semantic(std::shared_ptr<SrcFile> file) : file(file), scopeLevel(0), functionType(NONE) {
  this->symbolTable = std::make_shared<SymbolTable>(0);
}

void Semantic::analyse() {
  for (const auto &declaration : file->getAst()) {
    declaration->accept(this);
  }
}

void Semantic::analyse(const std::shared_ptr<Expr> &expr) {
  expr->accept(this);
}

void Semantic::analyse(const std::shared_ptr<Stmt> &stmt) { stmt->accept(this); }

void Semantic::analyse(const std::shared_ptr<Decl> &decl) { decl->accept(this); }

void Semantic::analyse(const std::vector<std::shared_ptr<Stmt>> &stmts) {
  for (const auto &stmt : stmts) {
    analyse(stmt);
  }
}

// TODO: improve scopes so Semantic does not have to know how SymbolTable is implemented
//  only SymbolTable.newScope(), SymbolTable.oldScope()
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

std::pair<bool, std::string> Semantic::isValidOperatorForType(const Token &op,
                                                              std::string typeOperands) {
  if (typeOperands == "int" || typeOperands == "float") {
    return isValidOperatorForNumericType(op, typeOperands);
  } else if (typeOperands == "string") {
    return isValidOperatorForStringType(op, typeOperands);
  } else if (typeOperands == "bool") {
    return isValidOperatorForBoolType(op, typeOperands);
  } else {
    // TODO: improve error handling
    std::cerr << "Type checking: type is not recognized " << std::endl;
    exit(1);
  }
}

std::pair<bool, std::string> Semantic::isValidOperatorForNumericType(const Token &op,
                                                                     std::string typeOperands) {
  switch (op.type) {
  case ADD:
  case SUB:
  case STAR:
  case SLASH:
    return std::make_pair(true, typeOperands);
  case EQUAL:
  case NOT_EQUAL:
  case LESS:
  case GREATER:
  case LESS_EQUAL:
  case GREATER_EQUAL:
    return std::make_pair(true, "bool");
  default:
    return std::make_pair(false, "");
  }
}

std::pair<bool, std::string> Semantic::isValidOperatorForStringType(const Token &op,
                                                                    std::string typeOperands) {
  switch (op.type) {
  // TODO: check if we can implement addition of strings in llvm ir
  case ADD:
    return std::make_pair(true, typeOperands);
  case EQUAL:
  case NOT_EQUAL:
    return std::make_pair(true, typeOperands);
  default:
    return std::make_pair(false, "");
  }
}

std::pair<bool, std::string> Semantic::isValidOperatorForBoolType(const Token &op,
                                                                  std::string typeOperands) {
  switch (op.type) {
  case LAND:
  case LOR:
  case NOT:
  case EQUAL:
  case NOT_EQUAL:
    return std::make_pair(true, typeOperands);
  default:
    return std::make_pair(false, "");
  }
}

// TODO: improve because we are using the token types from scanning and parsing in semantic analysis
//  which has drawbacks, like the repetition of the concept type for many things. Maybe create new
//  enum or class
//  Also change and add type to other AST nodes like VarDecl, ParamDecl, etc..
std::string Semantic::tokenTypeToType(TokenType tokenType) {
  switch (tokenType) {
  case LIT_TRUE:
  case LIT_FALSE:
    return "bool";
  case LIT_INT:
    return "int";
  case LIT_FLOAT:
    return "float";
  case LIT_STRING:
    return "string";
  default:
    // TODO: improve error handling
    std::cout << "token type not recognized" << std::endl;
    exit(1);
  }
}

void Semantic::visit(std::shared_ptr<VarDecl> node) {
  // Do semantic analysis of initializer expression (needed to calculate type)
  auto value = node->getValue();
  analyse(value);

  // Type checking
  if (node->getType().getTypeAsString() != node->getValue()->getType()) {
    // TODO: improve error handling
    std::cerr << "Type checking: different types " << node->getVarKeyword().value << " <l."
              << node->getVarKeyword().line << ":c." << node->getVarKeyword().column << ">"
              << std::endl;
    exit(1);
  }

  // Update information about if constant declaration is global
  bool isGlobal = this->scopeLevel == 0;
  node->setIsGlobal(isGlobal);

  // Update symbol table with new variable
  Symbol symbol(node->getIdentifier().value, Symbol::VARIABLE, node->getType().getTypeAsString());
  // TODO: check if this var int a = a + 1; is permitted if second a is in outer scope
  //  right now, we first insert and later assign so is permitted, maybe better to flip
  //  the following two actions
  symbolTable->insert(symbol.getIdentifier(), symbol);
}

void Semantic::visit(std::shared_ptr<ConstDecl> node) {
  // Do semantic analysis of initializer expression (needed to calculate type)
  analyse(node->getValue());

  // Type checking
  if (node->getType().getTypeAsString() != node->getValue()->getType()) {
    // TODO: improve error handling
    std::cerr << "Type checking: different types " << node->getConstKeyword().value << " <l."
              << node->getConstKeyword().line << ":c." << node->getConstKeyword().column << ">"
              << std::endl;
    exit(1);
  }

  // Update information about if constant declaration is global
  bool isGlobal = this->scopeLevel == 0;
  node->setIsGlobal(isGlobal);

  // Update symbol table with new constant
  Symbol symbol(node->getIdentifier().value, Symbol::CONSTANT, node->getType().getTypeAsString());
  // TODO: check if this var int a = a + 1; is permitted if second a is in outer scope
  //  right now, we first insert and later assign so is permitted, maybe better to flip
  //  the following two actions
  symbolTable->insert(symbol.getIdentifier(), symbol);
}

void Semantic::visit(std::shared_ptr<ParamDecl> node) {
  // Update symbol table with new constant
  Symbol symbol(node->getIdentifier().value, Symbol::PARAMETER, node->getType().getTypeAsString());
  symbolTable->insert(symbol.getIdentifier(), symbol);
}

void Semantic::visit(std::shared_ptr<FuncDecl> node) {
  Semantic::FunctionType previousFunctionType = functionType;
  functionType = Semantic::FUNCTION;
  // TODO: check if make returnType a class or enum instead of string
  returnType = node->isHasReturnType() ? node->getReturnType().getTypeAsString() : "void";

  // Need type of arguments to check later when function is called
  std::vector<std::string> parameterListType;
  for (const auto &parameter : node->getParams()) {
    parameterListType.push_back(parameter->getType().getTypeAsString());
  }

  // Insert function identifier in scope
  Symbol symbol(node->getIdentifier().value, Symbol::FUNCTION, returnType, parameterListType);
  symbolTable->insert(symbol.getIdentifier(), symbol);

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
  functionType = previousFunctionType;
  returnType = "";
}

void Semantic::visit(std::shared_ptr<DeclarationStmt> node) { analyse(node->getDecl()); }

void Semantic::visit(std::shared_ptr<ExpressionStmt> node) { analyse(node->getExpr()); }

void Semantic::visit(std::shared_ptr<BlockStmt> node) {
  beginScope();
  analyse(node->getStmts());
  endScope();
}

void Semantic::visit(std::shared_ptr<IfStmt> node) {
  analyse(node->getCondition());

  // Type checking for condition
  if (node->getCondition()->getType() != "bool") {
    // TODO: improve error handling
    std::cerr << "Type checking: different types in condition " << node->getIfKeyword().value
              << " <l." << node->getIfKeyword().line << ":c." << node->getIfKeyword().column << ">"
              << std::endl;
    exit(1);
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
  if (node->getCond()->getType() != "bool") {
    // TODO: improve error handling
    std::cerr << "Type checking: different types in condition " << node->getForKeyword().value
              << " <l." << node->getForKeyword().line << ":c." << node->getForKeyword().column
              << ">" << std::endl;
    exit(1);
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
  if (node->getCond()->getType() != "bool") {
    // TODO: improve error handling
    std::cerr << "Type checking: different types in condition " << node->getWhileKeyword().value
              << " <l." << node->getWhileKeyword().line << ":c." << node->getWhileKeyword().column
              << ">" << std::endl;
    exit(1);
  }

  beginScope();
  analyse(node->getBody());
  endScope();
}

void Semantic::visit(std::shared_ptr<AssignmentStmt> node) {
  analyse(node->getRhs());
  analyse(node->getLhs());

  // Type checking
  if (node->getRhs()->getType() != node->getLhs()->getType()) {
    // TODO: improve error handling
    std::cerr << "Type checking: different types " << node->getEqualToken().value << " <l."
              << node->getEqualToken().line << ":c." << node->getEqualToken().column << ">"
              << std::endl;
    exit(1);
  }
}

void Semantic::visit(std::shared_ptr<ReturnStmt> node) {
  if (functionType != Semantic::FUNCTION) {
    file->setErrorInSemanticAnalysis(true);
    // TODO: add file->addError() to add descriptive string to this message
  } else {
    analyse(node->getValue());

    // Type checking
    if (node->getValue()->getType() != this->returnType) {
      // TODO: improve error handling
      std::cerr << "Type checking: different types " << node->getReturnKeyword().value << " <l."
                << node->getReturnKeyword().line << ":c." << node->getReturnKeyword().column << ">"
                << std::endl;
      exit(1);
    }
  }
}

void Semantic::visit(std::shared_ptr<BinaryExpr> node) {
  analyse(node->getLhs());
  analyse(node->getRhs());

  // Type checking both operands are of same type
  if (node->getRhs()->getType() != node->getLhs()->getType()) {
    // TODO: improve error handling
    std::cerr << "Type checking: different types " << node->getOp().value << " <l."
              << node->getOp().line << ":c." << node->getOp().column << ">" << std::endl;
    exit(1);
  }

  // Type checking that operator is valid for types
  auto [isValid, type] = isValidOperatorForType(node->getOp(), node->getRhs()->getType());

  if (!isValid) {
    // TODO: improve error handling
    std::cerr << "Type checking: type is not compatible with operand " << node->getOp().value
              << " <l." << node->getOp().line << ":c." << node->getOp().column << ">" << std::endl;
    exit(1);
  }

  node->setType(type);
}

void Semantic::visit(std::shared_ptr<UnaryExpr> node) {
  analyse(node->getRhs());

  bool isValid = false;
  std::string type = "";

  if (node->getRhs()->getType() == "int" || node->getRhs()->getType() == "float") {
    std::tie(isValid, type) =
        isValidOperatorForNumericType(node->getOp(), node->getRhs()->getType());
  } else if (node->getRhs()->getType() == "string") {
    std::tie(isValid, type) =
        isValidOperatorForStringType(node->getOp(), node->getRhs()->getType());
  } else if (node->getRhs()->getType() == "bool") {
    std::tie(isValid, type) =
        isValidOperatorForNumericType(node->getOp(), node->getRhs()->getType());
  }

  if (!isValid) {
    // TODO: improve error handling
    std::cerr << "Type checking: type is not compatible with operand " << node->getOp().value
              << " <l." << node->getOp().line << ":c." << node->getOp().column << ">" << std::endl;
    exit(1);
  }

  node->setType(type);
}

void Semantic::visit(std::shared_ptr<LiteralExpr> node) {
  std::string type = tokenTypeToType(node->getToken().type);
  node->setType(type);
}

void Semantic::visit(std::shared_ptr<IdentExpr> node) {
  // TODO: improve and check only in current scope only so a = a+1; is not allowed
  Symbol symbol = symbolTable->lookup(node->getName());
  node->setType(symbol.getReturnType());

  // Update field to keep track of most recent resolved symbol for a function
  if(symbol.getType() == Symbol::FUNCTION) {
    resolvedSymbol = symbol;
  }
}

void Semantic::visit(std::shared_ptr<CallExpr> node) {
  // TODO: type checking of parameters
  analyse(node->getFunc());
  Symbol previousResolvedSymbol = resolvedSymbol;

  for (const auto &argument : node->getArgs()) {
    analyse(argument);
  }

  resolvedSymbol = previousResolvedSymbol;

  // Type checking for arguments
  if(node->getArgs().size() != resolvedSymbol.getParameterList().size()) {
    // TODO: improve error handling
    // TODO: improve fields of callExpr node to have some token to show where the error is
    std::cerr << "Type checking: number of arguments is different from number of parameters" << std::endl;
    exit(1);
  }

  auto itArgs = node->getArgs().begin();
  auto itParams = resolvedSymbol.getParameterList().begin();

  while(itArgs != node->getArgs().end() && itParams != resolvedSymbol.getParameterList().end()) {
    if((*itArgs)->getType() != (*itParams)) {
      // TODO: improve error handling
      std::cerr << "Type checking: type of parameters is different" << std::endl;
      exit(1);
    }
    itArgs++;
    itParams++;
  }

  node->setType(node->getFunc()->getType());
}