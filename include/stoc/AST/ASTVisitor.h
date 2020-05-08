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

// TODO: change the way includes are in AST to not have to declare all classes of nodes but instead include #include "Expr.h"
// declare all classes of nodes that exist in the AST
class BasicNode;
class Expr;
class ExprBinary;
class ExprUnary;
class ExprLiteral;

/// Base class from which other classes that traverse the AST will inherit.
/// The derived class must implement all of the methods that will depend on the type of node
class ASTVisitor {
public:
  virtual void visit(ExprBinary node) = 0;
  virtual void visit(ExprUnary node) = 0;
  virtual void visit(ExprLiteral node) = 0;
};

#endif // STOC_ASTPVISITOR_H
