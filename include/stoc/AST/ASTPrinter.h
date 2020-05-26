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

  void increaseDepthLevel();
  void lastChild();
  void decreaseDepthLevel();

public:
  ASTPrinter() = default;

  /// main method to print an AST from \ast node
  void print(const std::shared_ptr<BasicNode>& ast);

  // Visitor Pattern methods
  void visit(VarDecl node) override;
  void visit(ConstDecl node) override;
  void visit(ParamDecl node) override;
  void visit(FuncDecl node) override;

  void visit(ExpressionStmt node) override;
  void visit(DeclarationStmt node) override;
  void visit(BlockStmt node) override;
  void visit(IfStmt node) override;
  void visit(ForStmt node) override;
  void visit(WhileStmt node) override;
  void visit(AssignmentStmt node) override;
  void visit(ReturnStmt node) override;

  void visit(BinaryExpr node) override;
  void visit(UnaryExpr node) override;
  void visit(LiteralExpr node) override;
  void visit(IdentExpr node) override;
  void visit(CallExpr node) override;
};

#endif // STOC_ASTPRINTER_H
