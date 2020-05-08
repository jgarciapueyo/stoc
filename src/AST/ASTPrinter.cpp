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
#include "stoc/AST/Expr.h"

#include <iostream>

void ASTPrinter::print(const std::shared_ptr<BasicNode>& ast) {
  ast->accept(this);
};

void ASTPrinter::visit(ExprBinary node) {
  // print token binary operator
  // TODO: change to call node.getToken().printOneLine() or something similar
  std::cout << pre << "`-ExprBinary '"
            << node.getOp().type << "' "
            << "<l."<< node.getOp().line
            << ":c." << node.getOp().begin << ">" << std::endl;

  pre += "  |"; // add information to pre string (add 1 depth level)
  node.getLhs()->accept(this);
  pre = pre.substr(0, pre.length()-3); // restore pre string

  pre += "   "; // add information to pre string (add 1 depth level)
                // different from previous for being last child
  node.getRhs()->accept(this);
  pre = pre.substr(0, pre.size()-3); // restore pre string
};

void ASTPrinter::visit(ExprUnary node) {
  // print token unary operator
  // TODO: change to call node.getToken().printOneLine() or something similar
  std::cout << pre << "`-ExprUnary '"
            << node.getOp().type << "' "
            << "<l."<< node.getOp().line
            << ":c." << node.getOp().begin << ">" << std::endl;

  pre += "  |"; // add information to pre string (add 1 depth level)
  node.getRhs()->accept(this);
  pre = pre.substr(0,pre.size()-3); // restore pre string
};

void ASTPrinter::visit(ExprLiteral node) {
  // print literal token
  // TODO: change to call node.getToken().printOneLine() or something similar
  std::cout << pre << "`-ExprLiteral '"
            << node.getToken().value << "' "
            << node.getToken().type << " "
            << "<l."<< node.getToken().line
            << ":c." << node.getToken().begin << ">" << std::endl;
};