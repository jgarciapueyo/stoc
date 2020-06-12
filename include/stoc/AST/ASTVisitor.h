//===- stoc/AST/ASTVisitor.h - Defintion of basic visitor for AST --------*- C++ -*-===//
//
//===-------------------------------------------------------------------------------===//
//
// This file defines the ASTVisitor class.
// This is the base class from which other classes that traverse the AST will inherit.
//
//===------------------------------------------------------------------------------===//

#ifndef STOC_ASTVISITOR_H
#define STOC_ASTVISITOR_H

#include <memory>
// TODO: change the way includes are in AST/ to not have to declare all classes of nodes but instead
//  #include "Expr.h" "Decl.h" "Stmt.h" declare all classes of nodes that exist in the AST
class BasicNode;
class Decl;
class VarDecl;
class ConstDecl;
class ParamDecl;
class FuncDecl;

class Stmt;
class DeclarationStmt;
class ExpressionStmt;
class BlockStmt;
class IfStmt;
class ForStmt;
class WhileStmt;
class AssignmentStmt;
class ReturnStmt;

class Expr;
class BinaryExpr;
class UnaryExpr;
class LiteralExpr;
class IdentExpr;
class CallExpr;

/// Base class from which other classes that traverse the AST will inherit.
/// The derived class must implement all of the methods that will depend on the type of node
class ASTVisitor {
public:
  virtual void visit(std::shared_ptr<VarDecl> node) = 0;
  virtual void visit(std::shared_ptr<ConstDecl> node) = 0;
  virtual void visit(std::shared_ptr<ParamDecl> node) = 0;
  virtual void visit(std::shared_ptr<FuncDecl> node) = 0;

  virtual void visit(std::shared_ptr<DeclarationStmt> node) = 0;
  virtual void visit(std::shared_ptr<ExpressionStmt> node) = 0;
  virtual void visit(std::shared_ptr<BlockStmt> node) = 0;
  virtual void visit(std::shared_ptr<IfStmt> node) = 0;
  virtual void visit(std::shared_ptr<ForStmt> node) = 0;
  virtual void visit(std::shared_ptr<WhileStmt> node) = 0;
  virtual void visit(std::shared_ptr<AssignmentStmt> node) = 0;
  virtual void visit(std::shared_ptr<ReturnStmt> node) = 0;

  virtual void visit(std::shared_ptr<BinaryExpr> node) = 0;
  virtual void visit(std::shared_ptr<UnaryExpr> node) = 0;
  virtual void visit(std::shared_ptr<LiteralExpr> node) = 0;
  virtual void visit(std::shared_ptr<IdentExpr> node) = 0;
  virtual void visit(std::shared_ptr<CallExpr> node) = 0;
};

#endif // STOC_ASTPVISITOR_H
