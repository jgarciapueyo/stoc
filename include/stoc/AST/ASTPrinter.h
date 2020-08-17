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
  void print(const std::shared_ptr<BasicNode> &ast);

  // Visitor Pattern methods
  void visit(std::shared_ptr<VarDecl> node) override;
  void visit(std::shared_ptr<ConstDecl> node) override;
  void visit(std::shared_ptr<ParamDecl> node) override;
  void visit(std::shared_ptr<FuncDecl> node) override;

  void visit(std::shared_ptr<DeclarationStmt> node) override;
  void visit(std::shared_ptr<ExpressionStmt> node) override;
  void visit(std::shared_ptr<BlockStmt> node) override;
  void visit(std::shared_ptr<IfStmt> node) override;
  void visit(std::shared_ptr<ForStmt> node) override;
  void visit(std::shared_ptr<WhileStmt> node) override;
  void visit(std::shared_ptr<AssignmentStmt> node) override;
  void visit(std::shared_ptr<ReturnStmt> node) override;

  void visit(std::shared_ptr<BinaryExpr> node) override;
  void visit(std::shared_ptr<UnaryExpr> node) override;
  void visit(std::shared_ptr<LiteralExpr> node) override;
  void visit(std::shared_ptr<IdentExpr> node) override;
  void visit(std::shared_ptr<CallExpr> node) override;
};

#endif // STOC_ASTPRINTER_H
