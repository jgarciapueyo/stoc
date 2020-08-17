//===- src/AST/ASTPrinter.cpp - Implementation of a printer for AST -----------------*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements the ASTPrinter class.
// It prints the AST in a pretty way with information about every node->
//
//===------------------------------------------------------------------------------------------===//

#include "stoc/AST/ASTPrinter.h"

#include <iostream>

#include "stoc/AST/BasicNode.h"
#include "stoc/AST/Decl.h"
#include "stoc/AST/Expr.h"
#include "stoc/AST/Stmt.h"

void ASTPrinter::increaseDepthLevel() {
  if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
    pre.at(pre.length() - 1) = ' ';
  }

  pre += " |"; // add information to pre string (add 1 depth level)
}

void ASTPrinter::lastChild() {
  pre = pre.substr(0, pre.length() - 2); // restore pre string

  pre += " `"; // add information to pre string (add 1 depth level)
  // different from previous for being last child
}

void ASTPrinter::decreaseDepthLevel() {
  pre = pre.substr(0, pre.size() - 2); // restore pre string
}

void ASTPrinter::print(const std::shared_ptr<BasicNode> &ast) { ast->accept(this); }

void ASTPrinter::visit(std::shared_ptr<VarDecl> node) {
  // print variable declaration token
  std::cout << pre << "-VarDecl <l." << node->getVarKeywordToken().line << ":c."
            << node->getVarKeywordToken().column << "> '" << node->getIdentifierToken().value
            << "' " << node->getTypeToken().tokenType << std::endl;

  increaseDepthLevel();
  lastChild();
  node->getValue()->accept(this);
  decreaseDepthLevel();
}

void ASTPrinter::visit(std::shared_ptr<ConstDecl> node) {
  // print constant declaration token
  std::cout << pre << "-ConstDecl <l." << node->getConstKeywordToken().line << ":c."
            << node->getConstKeywordToken().column << "> '" << node->getIdentifierToken().value
            << "' " << node->getTypeToken().tokenType << std::endl;

  increaseDepthLevel();
  lastChild();
  node->getValue()->accept(this);
  decreaseDepthLevel();
}

void ASTPrinter::visit(std::shared_ptr<ParamDecl> node) {
  std::cout << pre << "-ParamDecl <l." << node->getKeywordToken().line << ":c."
            << node->getKeywordToken().column << "> '" << node->getIdentifierToken().value << "' "
            << node->getTypeToken().tokenType << std::endl;
}

void ASTPrinter::visit(std::shared_ptr<FuncDecl> node) {
  std::cout << pre << "-FuncDecl <l." << node->getFuncKeywordToken().line << ":c"
            << node->getFuncKeywordToken().column << "> '" << node->getIdentifierToken().value
            << "' ";

  if (node->isHasReturnType()) {
    std::cout << node->getReturnTypeToken().tokenType;
  }
  std::cout << std::endl;

  increaseDepthLevel();

  int size = node->getParams().size();
  for (int i = 0; i < size; i++) {
    node->getParams().at(i)->accept(this);
  }

  lastChild();
  node->getBody()->accept(this);
  decreaseDepthLevel();
}

void ASTPrinter::visit(std::shared_ptr<BinaryExpr> node) {
  std::cout << pre << "-BinaryExpr <l." << node->getOp().line << ":c." << node->getOp().column
            << "> " << node->getOp().tokenType << " " << node->getType() << std::endl;

  increaseDepthLevel();
  node->getLhs()->accept(this);
  lastChild();
  node->getRhs()->accept(this);
  decreaseDepthLevel();
}

void ASTPrinter::visit(std::shared_ptr<UnaryExpr> node) {
  std::cout << pre << "-UnaryExpr <l." << node->getOp().line << ":c." << node->getOp().column
            << "> " << node->getOp().tokenType << " " << node->getType() << std::endl;

  increaseDepthLevel();
  lastChild();
  node->getRhs()->accept(this);
  decreaseDepthLevel();
}

void ASTPrinter::visit(std::shared_ptr<LiteralExpr> node) {
  std::cout << pre << "-LiteralExpr <l." << node->getToken().line << ":c."
            << node->getToken().column << "> " << node->getToken().tokenType
            << " '" <<  node->getToken().value << "' " << node->getType() << std::endl;
}

void ASTPrinter::visit(std::shared_ptr<IdentExpr> node) {
  std::cout << pre << "-IdentExpr <l." << node->getIdent().line << ":c." << node->getIdent().column
            << "> '" << node->getName() << "'"
            << " " << node->getType() << std::endl;
}

void ASTPrinter::visit(std::shared_ptr<CallExpr> node) {
  std::cout << pre << "-CallExpr " << node->getType() << std::endl;

  int size = node->getArgs().size();
  if (size > 0) {
    increaseDepthLevel();
    node->getFunc()->accept(this);

    for (int i = 0; i < size - 1; i++) {
      node->getArgs().at(i)->accept(this);
    }

    lastChild();
    node->getArgs().at(size - 1)->accept(this);
    decreaseDepthLevel();
  } else {
    increaseDepthLevel();
    lastChild();
    node->getFunc()->accept(this);
    decreaseDepthLevel();
  }
}

void ASTPrinter::visit(std::shared_ptr<ExpressionStmt> node) {
  std::cout << pre << "-ExpressionStmt" << std::endl;

  increaseDepthLevel();
  lastChild();
  node->getExpr()->accept(this);
  decreaseDepthLevel();
}

void ASTPrinter::visit(std::shared_ptr<DeclarationStmt> node) {
  std::cout << pre << "-DeclarationStmt" << std::endl;

  increaseDepthLevel();
  lastChild();
  node->getDecl()->accept(this);
  decreaseDepthLevel();
}

void ASTPrinter::visit(std::shared_ptr<BlockStmt> node) {
  std::cout << pre << "-BlockStmt <l." << node->getLbrace().line << ":c."
            << node->getLbrace().column << "> - <l." << node->getRbrace().line << ":c."
            << node->getRbrace().column << ">" << std::endl;

  int size = node->getStmts().size();
  if (size > 0) {
    increaseDepthLevel();
    for (int i = 0; i < size - 1; i++) {
      node->getStmts().at(i)->accept(this);
    }

    lastChild();
    node->getStmts().at(node->getStmts().size() - 1)->accept(this);
    decreaseDepthLevel();
  }
}

void ASTPrinter::visit(std::shared_ptr<IfStmt> node) {
  std::cout << pre << "-IfStmt <l." << node->getIfKeyword().line << ":c."
            << node->getIfKeyword().column << ">" << std::endl;

  increaseDepthLevel();

  node->getCondition()->accept(this);

  if (node->isHasElse()) {
    node->getThenBranch()->accept(this);
    lastChild();
    node->getElseBranch()->accept(this);
    decreaseDepthLevel();
  } else {
    lastChild();
    node->getThenBranch()->accept(this);
    decreaseDepthLevel();
  }
}

void ASTPrinter::visit(std::shared_ptr<ForStmt> node) {
  std::cout << pre << "-ForStmt <l." << node->getForKeyword().line << ":c."
            << node->getForKeyword().column << ">" << std::endl;

  increaseDepthLevel();

  if (node->getInit() != nullptr) {
    node->getInit()->accept(this);
  } else {
    std::cout << pre << "- <<no initialization>>" << std::endl;
  }

  if (node->getCond() != nullptr) {
    node->getCond()->accept(this);
  } else {
    std::cout << pre << "- <<no condition>>" << std::endl;
  }

  if (node->getPost() != nullptr) {
    node->getPost()->accept(this);
  } else {
    std::cout << pre << "- <<no post statement>>" << std::endl;
  }

  lastChild();
  node->getBody()->accept(this);
  decreaseDepthLevel();
}

void ASTPrinter::visit(std::shared_ptr<WhileStmt> node) {
  std::cout << pre << "-WhileStmt <l." << node->getWhileKeyword().line << ":c."
            << node->getWhileKeyword().column << ">" << std::endl;

  increaseDepthLevel();

  node->getCond()->accept(this);
  lastChild();
  node->getBody()->accept(this);
  decreaseDepthLevel();
}

void ASTPrinter::visit(std::shared_ptr<AssignmentStmt> node) {
  std::cout << pre << "-AssignmentStmt <l." << node->getEqualToken().line << ":c."
            << node->getEqualToken().column << ">" << std::endl;

  increaseDepthLevel();
  node->getLhs()->accept(this);
  lastChild();
  node->getRhs()->accept(this);
  decreaseDepthLevel();
}

void ASTPrinter::visit(std::shared_ptr<ReturnStmt> node) {
  std::cout << pre << "-ReturnStmt <l." << node->getReturnKeyword().line << ":c."
            << node->getReturnKeyword().column << ">" << std::endl;

  if (node->getValue() != nullptr) {
    increaseDepthLevel();
    lastChild();
    node->getValue()->accept(this);
    decreaseDepthLevel();
  }
}
