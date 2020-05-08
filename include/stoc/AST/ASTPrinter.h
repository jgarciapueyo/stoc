//===- stoc/AST/ASTPrinter.h - Definition of a printer for AST --------*- C++ -*-===//
//
//===----------------------------------------------------------------------------===//
//
// This file defines the ASTPrinter class.
// It prints the AST in a pretty way with information about every node.
//
//===---------------------------------------------------------------------------===//

#ifndef STOC_ASTPRINTER_H
#define STOC_ASTPRINTER_H

#include "ASTVisitor.h"

#include <memory>
#include <string>

/// Prints the AST in a pretty way with information about every node
class ASTPrinter : public ASTVisitor {
private:

  /// string appended before printing a given node. It also allows to know the depth of the node
  std::string pre = "";

public:
  ASTPrinter() = default;

  /// main method to print an AST from \ast node
  void print(const std::shared_ptr<BasicNode>& ast);

  // Visitor Pattern methods
  void visit(ExprBinary node) override;
  void visit(ExprUnary node) override;
  void visit(ExprLiteral node) override;
};

#endif // STOC_ASTPRINTER_H
