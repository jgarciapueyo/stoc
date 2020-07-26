//===- stoc/AST/BasicNode.h - Defintion of BasicNode of AST --------*- C++ -*-===//
//
//===------------------------------------------------------------------------===//
//
// This file defines the BasicNode class.
// This is the base class from which other nodes of the AST will inherit.
//
//===------------------------------------------------------------------------===//

#ifndef STOC_BASICNODE_H
#define STOC_BASICNODE_H

#include <iostream>

#include "stoc/AST/ASTVisitor.h"

/// Base class from which other nodes of the AST will inherit.
/// It implements the Visitor Pattern for adding new operations to the AST nodes
class BasicNode {
public:
  /// method needed for the Visitor Pattern
  virtual void accept(ASTVisitor *visitor) = 0;
};

#endif // STOC_BASICNODE_H
