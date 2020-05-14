//===- stoc/AST/ASTPrinter.cpp - Implementation of a printer for AST --------*- C++ -*-===//
//
//===----------------------------------------------------------------------------------===//
//
// This file implements the ASTPrinter class.
// It prints the AST in a pretty way with information about every node.
//
//===---------------------------------------------------------------------------------===//

#include "stoc/AST/ASTPrinter.h"

#include "stoc/AST/BasicNode.h"
#include "stoc/AST/Decl.h"
#include "stoc/AST/Expr.h"
#include "stoc/AST/Stmt.h"

#include <iostream>

void ASTPrinter::print(const std::shared_ptr<BasicNode> &ast) { ast->accept(this); };

void ASTPrinter::visit(BinaryExpr node) {
  // print token binary operator
  // TODO: change to call node.getToken().printOneLine() or something similar
  std::cout << pre << "-BinaryExpr '" << node.getOp().type << "' "
            << "<l." << node.getOp().line << ":c." << node.getOp().begin << ">" << std::endl;

  if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
    pre.at(pre.length() - 1) = ' ';
  }

  pre += " |"; // add information to pre string (add 1 depth level)
  node.getLhs()->accept(this);
  pre = pre.substr(0, pre.length() - 2); // restore pre string

  pre += " `"; // add information to pre string (add 1 depth level)
               // different from previous for being last child
  node.getRhs()->accept(this);
  pre = pre.substr(0, pre.size() - 2); // restore pre string
};

void ASTPrinter::visit(UnaryExpr node) {
  // print token unary operator
  // TODO: change to call node.getToken().printOneLine() or something similar
  std::cout << pre << "-UnaryExpr '" << node.getOp().type << "' "
            << "<l." << node.getOp().line << ":c." << node.getOp().begin << ">" << std::endl;

  if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
    pre.at(pre.length() - 1) = ' ';
  }

  pre += " `"; // add information to pre string (add 1 depth level)
  node.getRhs()->accept(this);
  pre = pre.substr(0, pre.size() - 2); // restore pre string
};

void ASTPrinter::visit(LiteralExpr node) {
  // print literal token
  // TODO: change to call node.getToken().printOneLine() or something similar
  std::cout << pre << "-LiteralExpr '" << node.getToken().value << "' " << node.getToken().type
            << " "
            << "<l." << node.getToken().line << ":c." << node.getToken().begin << ">" << std::endl;
};

void ASTPrinter::visit(IdentExpr node) {
  // print identifier token
  std::cout << pre << "-IdentExpr '" << node.getIdent().value << "' <l." << node.getIdent().line
            << ":c." << node.getIdent().begin << ">" << std::endl;
}

void ASTPrinter::visit(CallExpr node) {
  std::cout << pre << "-CallExpr" << std::endl;

  int size = node.getArgs().size();
  if(size > 0) {
    if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
      pre.at(pre.length() - 1) = ' ';
    }
    pre += " |"; // add information to pre string (add 1 depth level)
    node.getFunc()->accept(this);
    pre = pre.substr(0, pre.length() - 2); // restore pre string

    for(int i = 0; i < size - 1; i++) {
      node.getArgs().at(i)->accept(this);
    }

    pre += " `"; // add information to pre string (add 1 depth level)
    node.getArgs().at(size - 1)->accept(this);
    pre = pre.substr(0, pre.size() - 2); // restore pre string
  }
  else {
    if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
      pre.at(pre.length() - 1) = ' ';
    }
    pre += " `"; // add information to pre string (add 1 depth level)
    node.getFunc()->accept(this);
    pre = pre.substr(0, pre.length() - 2); // restore pre string
  }
}

void ASTPrinter::visit(VarDecl node) {
  // print variable declaration token
  // TODO: change to call node.getToken().printOneLine() or something similar
  std::cout << pre << "-VarDecl '" << node.getIdentifier() << "' "
            << node.getType().getTypeAsString() << " "
            << "<l." << node.getType().line << ":c." << node.getType().begin << ">" << std::endl;

  if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
    pre.at(pre.length() - 1) = ' ';
  }
  pre += " `"; // add information to pre string (add 1 depth level)
  node.getValue()->accept(this);
  pre = pre.substr(0, pre.size() - 2); // restore pre string
};

void ASTPrinter::visit(ConstDecl node) {
  // print constant declaration token
  // TODO: change to call node.getToken().printOneLine() or something similar
  std::cout << pre << "-VarDecl '" << node.getIdentifier() << "' "
            << node.getType().getTypeAsString() << " "
            << "<l." << node.getType().line << ":c." << node.getType().begin << ">" << std::endl;

  if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
    pre.at(pre.length() - 1) = ' ';
  }
  pre += " `"; // add information to pre string (add 1 depth level)
  node.getValue()->accept(this);
  pre = pre.substr(0, pre.size() - 2); // restore pre string
}

void ASTPrinter::visit(ParamDecl node) {
  std::cout << pre << "-ParamDecl '" << node.getName().value << "' "
            << node.getType().getTypeAsString() << " "
            << "<l." << node.getType().line << ":c." << node.getType().begin << ">" << std::endl;
}

void ASTPrinter::visit(FuncDecl node) {
  std::cout << pre << "-FuncDecl" << std::endl;

  pre += " |"; // add information to pre string (add 1 depth level)

  int size = node.getParams().size();
  for (int i = 0; i < size; i++) {
    node.getParams().at(i)->accept(this);
  }
  pre = pre.substr(0, pre.length() - 2); // restore pre string
  pre += " `"; // add information to pre string (add 1 depth level)
  // different from previous for being last child
  node.getBody()->accept(this);
  pre = pre.substr(0, pre.size() - 2); // restore pre string
}

void ASTPrinter::visit(ExpressionStmt node) {
  std::cout << pre << "-ExpressionStmt" << std::endl;

  if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
    pre.at(pre.length() - 1) = ' ';
  }
  pre += " `"; // add information to pre string (add 1 depth level)
  node.getExpr()->accept(this);
  pre = pre.substr(0, pre.size() - 2); // restore pre string
};

void ASTPrinter::visit(DeclarationStmt node) {
  std::cout << pre << "-DeclarationStmt" << std::endl;

  if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
    pre.at(pre.length() - 1) = ' ';
  }
  pre += " `"; // add information to pre string (add 1 depth level)
  node.getDecl()->accept(this);
  pre = pre.substr(0, pre.size() - 2); // restore pre string
}

void ASTPrinter::visit(BlockStmt node) {
  std::cout << pre << "-BlockStmt" << std::endl;

  if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
    pre.at(pre.length() - 1) = ' ';
  }
  pre += " |"; // add information to pre string (add 1 depth level)

  int size = node.getStmts().size();
  for (int i = 0; i < size - 1; i++) {
    node.getStmts().at(i)->accept(this);
  }

  pre = pre.substr(0, pre.length() - 2); // restore pre string

  if (size > 0) {
    pre += " `"; // add information to pre string (add 1 depth level)
                 // different from previous for being last child
    node.getStmts().at(node.getStmts().size() - 1)->accept(this);
    pre = pre.substr(0, pre.size() - 2); // restore pre string
  }
}

void ASTPrinter::visit(IfStmt node) {
  std::cout << pre << "-IfStmt" << std::endl;
  // TODO: add description of Token if

  if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
    pre.at(pre.length() - 1) = ' ';
  }
  pre += " |"; // add information to pre string (add 1 depth level)

  node.getCondition()->accept(this);

  if (node.isHasElse()) {
    node.getThenBranch()->accept(this);
    pre = pre.substr(0, pre.length() - 2); // restore pre string

    pre += " `"; // add information to pre string (add 1 depth level)
                 // different from previous for being last child
    node.getElseBranch()->accept(this);
    pre = pre.substr(0, pre.size() - 2); // restore pre string
  } else {
    pre = pre.substr(0, pre.length() - 2); // restore pre string

    pre += " `"; // add information to pre string (add 1 depth level)
    // different from previous for being last child
    node.getThenBranch()->accept(this);
    pre = pre.substr(0, pre.size() - 2); // restore pre string
  }
}

void ASTPrinter::visit(ForStmt node) {
  std::cout << pre << "-ForStmt" << std::endl;
  // TODO: add description of Token for

  if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
    pre.at(pre.length() - 1) = ' ';
  }
  pre += " |"; // add information to pre string (add 1 depth level)

  if(node.getInit() != nullptr) {
    node.getInit()->accept(this);
  }
  else {
    std::cout << pre << "- <<no initialization>>" << std::endl;
  }

  if(node.getCond() != nullptr) {
    node.getCond()->accept(this);
  }
  else {
    std::cout << pre << "- <<no condition>>" << std::endl;
  }

  if(node.getPost() != nullptr) {
    node.getPost()->accept(this);
  }
  else {
    std::cout << pre << "- <<no post statement>>" << std::endl;
  }

  pre = pre.substr(0, pre.length() - 2); // restore pre string
  pre += " `"; // add information to pre string (add 1 depth level)
               // different from previous for being last child
  node.getBody()->accept(this);
  pre = pre.substr(0, pre.size() - 2); // restore pre string
}

void ASTPrinter::visit(WhileStmt node) {
  std::cout << pre << "-WhileStmt" << std::endl;
  // TODO: add description of Token while
  if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
    pre.at(pre.length() - 1) = ' ';
  }
  pre += " |"; // add information to pre string (add 1 depth level)

  node.getCond()->accept(this);
  pre = pre.substr(0, pre.length() - 2); // restore pre string
  pre += " `"; // add information to pre string (add 1 depth level)
  // different from previous for being last child
  node.getBody()->accept(this);
  pre = pre.substr(0, pre.size() - 2); // restore pre string
}

void ASTPrinter::visit(AssignmenStmt node) {
  std::cout << pre << "-AssignmentStmt" << std::endl;

  if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
    pre.at(pre.length() - 1) = ' ';
  }

  pre += " |"; // add information to pre string (add 1 depth level)
  node.getLhs()->accept(this);
  pre = pre.substr(0, pre.length() - 2); // restore pre strin
  pre += " `"; // add information to pre string (add 1 depth level)
               // different from previous for being last child
  node.getRhs()->accept(this);
  pre = pre.substr(0, pre.size() - 2); // restore pre string
}

void ASTPrinter::visit(ReturnStmt node) {
  std::cout << pre << "-ReturnStmt" << std::endl;

  if(node.getValue() != nullptr) {
    if (pre.length() > 0 && pre.at(pre.length() - 1) == '`') {
      pre.at(pre.length() - 1) = ' ';
    }

    pre += " `"; // add information to pre string (add 1 depth level)
    // different from previous for being last child
    node.getValue()->accept(this);
    pre = pre.substr(0, pre.size() - 2); // restore pre string


  }
}
