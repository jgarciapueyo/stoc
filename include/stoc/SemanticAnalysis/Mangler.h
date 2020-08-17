//===- stoc/SemanticAnalysis/Mangler.h - Defintion of mangler function -------------*- C++ -*-===//
//
//===-----------------------------------------------------------------------------------------===//
//
// This file defines the mangler function used for modifying functions declarations identifier to
// allow for overloading functions in Stoc by adding information about the parameter types.
// This is needed in Code Generation for LLVM IR because it does not support function overloading
// and function identifiers in LLVM IR must be different.
//
//===-----------------------------------------------------------------------------------------===//
#ifndef STOC_MANGLER_H
#define STOC_MANGLER_H

#include <string>

#include "stoc/SemanticAnalysis/Type.h"

namespace mangler {

/// returns the identifier of the function modified with format:
/// (functionName)_(numberofparameters)p_[typeparameter]*_r(typereturn)
/// i.e. func foo(var int a, var float b) bool -> foo_2p_intfloat_rbool
std::string mangle(std::string functionName, std::shared_ptr<FunctionType> functionType);

}

#endif // STOC_MANGLER_H
