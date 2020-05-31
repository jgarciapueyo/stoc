// TODO: add header of the file
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

void Semantic::analyse(std::shared_ptr<Expr> expr) { expr->accept(this); }

void Semantic::analyse(std::shared_ptr<Stmt> stmt) { stmt->accept(this); }

void Semantic::analyse(std::shared_ptr<Decl> decl) { decl->accept(this); }

void Semantic::analyse(std::vector<std::shared_ptr<Stmt>> stmts) {
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

void Semantic::visit(VarDecl node) {
  Symbol symbol(node.getIdentifier().value, Symbol::VARIABLE, node.getType().getTypeAsString());
  // TODO: check if this var int a = a + 1; is permitted if second a is in outer scope
  //  right now, we first insert and later assign so is permitted, maybe better to flip
  //  the following two actions
  symbolTable->insert(symbol.getIdentifier(), symbol);
  analyse(node.getValue());
}

void Semantic::visit(ConstDecl node) {
  Symbol symbol(node.getIdentifier().value, Symbol::CONSTANT, node.getType().getTypeAsString());
  symbolTable->insert(symbol.getIdentifier(), symbol);
  analyse(node.getValue());
}

void Semantic::visit(ParamDecl node) {
  Symbol symbol(node.getIdentifier().value, Symbol::PARAMETER, node.getType().getTypeAsString());
  symbolTable->insert(symbol.getIdentifier(), symbol);
}

void Semantic::visit(FuncDecl node) {

  Semantic::FunctionType previousFunctionType = functionType;
  functionType = Semantic::FUNCTION;

  // Need type of arguments to check later when function is called
  std::vector<std::string> parameterListType;
  for (const auto &parameter : node.getParams()) {
    parameterListType.push_back(parameter->getType().getTypeAsString());
  }

  // Insert function identifier in scope
  std::string returnType = node.isHasReturnType() ? node.getReturnType().getTypeAsString() : "void";
  Symbol symbol(node.getIdentifier().value, Symbol::FUNCTION, returnType, parameterListType);
  symbolTable->insert(symbol.getIdentifier(), symbol);

  // Analyse parameters and body of the function
  beginScope();
  for (const auto &parameter : node.getParams()) {
    analyse(parameter);
  }

  // In FuncDecl, declarations of parameters have to be inside the scope of the body so the scope
  //  it is created in the beginning. Because the scope has been already created, we call directly
  //  to analyse the vector of statements since analyse block of statements creates a new scope
  //  and we do not want a new scope
  analyse(node.getBody()->getStmts());
  endScope();

  functionType = previousFunctionType;
}

void Semantic::visit(DeclarationStmt node) { analyse(node.getDecl()); }

void Semantic::visit(ExpressionStmt node) { analyse(node.getExpr()); }

void Semantic::visit(BlockStmt node) {
  beginScope();
  analyse(node.getStmts());
  endScope();
}

void Semantic::visit(IfStmt node) {
  analyse(node.getCondition());

  analyse(node.getThenBranch());

  if (node.isHasElse()) {
    analyse(node.getElseBranch());
  }
}

void Semantic::visit(ForStmt node) {
  beginScope();
  analyse(node.getInit());
  analyse(node.getCond());
  analyse(node.getPost());
  // In ForStmt, declarations in init have to be inside the scope of the body so the scope it is
  //  created in the beginning. Because the scope has been already created, we call directly to
  //  analyse the vector of statements since analyse block of statements creates a new scope
  //  and we do not want a new scope
  analyse(node.getBody()->getStmts());
  endScope();
}

void Semantic::visit(WhileStmt node) {

  analyse(node.getCond());
  beginScope();
  analyse(node.getBody());
  endScope();
}

void Semantic::visit(AssignmentStmt node) {
  analyse(node.getRhs());
  analyse(node.getLhs());
}

void Semantic::visit(ReturnStmt node) {
  if (functionType != Semantic::FUNCTION) {
    file->setErrorInSemanticAnalysis(true);
    // TODO: add file->addError() to add descriptive string to this message
  } else {
    analyse(node.getValue());
  }
}

void Semantic::visit(BinaryExpr node) {
  analyse(node.getLhs());
  analyse(node.getRhs());
}

void Semantic::visit(UnaryExpr node) { analyse(node.getRhs()); }

void Semantic::visit(LiteralExpr node) {}

void Semantic::visit(IdentExpr node) {
  // TODO: improve and check only in current scope only so a = a+1; is not allowed
  symbolTable->lookup(node.getName());
}

void Semantic::visit(CallExpr node) {
  analyse(node.getFunc());

  for (const auto &argument : node.getArgs()) {
    analyse(argument);
  }
}