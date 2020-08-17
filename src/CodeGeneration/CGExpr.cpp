//=== src/CodeGeneration/CGExpr.cpp - Impl of CodeGeneration for Expressions --------*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements the methods of the CodeGeneration class used to generate code
// for Expressions.
//
//===------------------------------------------------------------------------------------------===//
#include "stoc/CodeGeneration/CodeGeneration.h"

llvm::Value *CodeGeneration::generate(const std::shared_ptr<Expr> &node) {
  switch (node->getExprKind()) {
  case Expr::Kind::BINARYEXPR:
    return generate(std::static_pointer_cast<BinaryExpr>(node));
  case Expr::Kind::UNARYEXPR:
    return generate(std::static_pointer_cast<UnaryExpr>(node));
  case Expr::Kind::LITERALEXPR:
    return generate(std::static_pointer_cast<LiteralExpr>(node));
  case Expr::Kind::IDENTEXPR:
    return generate(std::static_pointer_cast<IdentExpr>(node));
  case Expr::Kind::CALLEXPR:
    return generate(std::static_pointer_cast<CallExpr>(node));
  default:
    reportError("Internal Error - Expression kind not allowed");
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generateBinaryExprInt(const std::shared_ptr<BinaryExpr> &node,
                                                   llvm::Value *lhs, llvm::Value *rhs) {
  switch (node->getOp().tokenType) {
  case ADD:
    return builder->CreateAdd(lhs, rhs, "addtemp");
  case SUB:
    return builder->CreateSub(lhs, rhs, "subtemp");
  case STAR:
    return builder->CreateMul(lhs, rhs, "multemp");
  case SLASH:
    return builder->CreateSDiv(lhs, rhs, "divtemp");
  case EQUAL:
    return builder->CreateICmpEQ(lhs, rhs, "equaltmp");
  case NOT_EQUAL:
    return builder->CreateICmpNE(lhs, rhs, "nequaltmp");
  case LESS:
    return builder->CreateICmpSLT(lhs, rhs, "lesstmp");
  case GREATER:
    return builder->CreateICmpSGT(lhs, rhs, "greatertmp");
  case LESS_EQUAL:
    return builder->CreateICmpSLE(lhs, rhs, "lesseqtmp");
  case GREATER_EQUAL:
    return builder->CreateICmpSGE(lhs, rhs, "greatereqtmp");
  default:
    reportError("Internal Error - Binary Operator not allowed for int type", node->getOp().line,
                node->getOp().column);
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generateBinaryExprFloat(const std::shared_ptr<BinaryExpr> &node,
                                                     llvm::Value *lhs, llvm::Value *rhs) {
  switch (node->getOp().tokenType) {
  case ADD:
    return builder->CreateFAdd(lhs, rhs, "addtemp");
  case SUB:
    return builder->CreateFSub(lhs, rhs, "subtemp");
  case STAR:
    return builder->CreateFMul(lhs, rhs, "multemp");
  case SLASH:
    return builder->CreateFDiv(lhs, rhs, "divtemp");
  case EQUAL:
    return builder->CreateFCmpOEQ(lhs, rhs, "equaltmp");
  case NOT_EQUAL:
    return builder->CreateFCmpONE(lhs, rhs, "nequaltmp");
  case LESS:
    return builder->CreateFCmpOLT(lhs, rhs, "lesstmp");
  case GREATER:
    return builder->CreateFCmpOGT(lhs, rhs, "greatertmp");
  case LESS_EQUAL:
    return builder->CreateFCmpOLE(lhs, rhs, "lesseqtmp");
  case GREATER_EQUAL:
    return builder->CreateFCmpOGE(lhs, rhs, "greatereqtmp");
  default:
    reportError("Internal Error - Binary Operator not allowed for float type", node->getOp().line,
                node->getOp().column);
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generateBinaryExprBool(const std::shared_ptr<BinaryExpr> &node,
                                                    llvm::Value *lhs, llvm::Value *rhs) {
  // TODO: (improvement) see how to do it for shortcircuit logical operators
  switch (node->getOp().tokenType) {
  case EQUAL:
    return builder->CreateICmpEQ(lhs, rhs, "cmpeqtmp");
  case NOT_EQUAL:
    return builder->CreateICmpNE(lhs, rhs, "cmpnetmp");
  case LAND:
    return builder->CreateAnd(lhs, rhs, "andtemp");
  case LOR:
    return builder->CreateOr(lhs, rhs, "ortemp");
  default:
    reportError("Internal Error - Binary Operator not allowed for bool type", node->getOp().line,
                node->getOp().column);
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generateBinaryExprString(const std::shared_ptr<BinaryExpr> &node,
                                                      llvm::Value *lhs, llvm::Value *rhs) {
  switch (node->getOp().tokenType) {
  case EQUAL: {
    std::vector<llvm::Value *> args{lhs, rhs};
    llvm::Function *callee = module->getFunction("strcmp");
    auto *callInst = builder->CreateCall(callee, args, "calltmp");
    return builder->CreateICmpEQ(callInst, llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0));
  }
  case NOT_EQUAL: {
    std::vector<llvm::Value *> args{lhs, rhs};
    llvm::Function *callee = module->getFunction("strcmp");
    auto *callInst = builder->CreateCall(callee, args, "calltmp");
    return builder->CreateICmpNE(callInst,
                                 llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0));
  }
  default:
    reportError("Internal Error - Binary Operator not allowed for string type", node->getOp().line,
                node->getOp().column);
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generate(const std::shared_ptr<BinaryExpr> &node) {
  llvm::Value *lhs = CodeGeneration::generate(node->getLhs());
  llvm::Value *rhs = CodeGeneration::generate(node->getRhs());

  // To decide the type, we use one of the child's type because that is the type of the operands
  // The type of the node is the type after applying the binary operator
  switch (node->getLhs()->getType()->getTypeKind()) {
  case Type::Kind::BasicType: {
    auto type = std::dynamic_pointer_cast<BasicType>(node->getLhs()->getType());
    switch (type->getKind()) {
    case BasicType::Kind::INT:
      return generateBinaryExprInt(node, lhs, rhs);
    case BasicType::Kind::FLOAT:
      return generateBinaryExprFloat(node, lhs, rhs);
    case BasicType::Kind::BOOL:
      return generateBinaryExprBool(node, lhs, rhs);
    case BasicType::Kind::STRING:
      return generateBinaryExprString(node, lhs, rhs);
    default:
      reportError("Internal Error - Binary Operator not allowed for type not known",
                  node->getOp().line, node->getOp().column);
      return nullptr;
    }
  }
  case Type::Kind::Signature:
    reportError("Internal Error - Binary Operator can not be of type function", node->getOp().line,
                node->getOp().column);
    return nullptr;
  default:
    reportError("Internal Error - Binary Operator of type not known", node->getOp().line,
                node->getOp().column);
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generateUnaryExprInt(const std::shared_ptr<UnaryExpr> &node,
                                                  llvm::Value *rhs) {
  switch (node->getOp().tokenType) {
  case ADD:
    return rhs; // if +Expr is the same as Expr
  case SUB:
    return builder->CreateNeg(rhs);
  default:
    reportError("Internal Error - Unary Operator not allowed for int type", node->getOp().line,
                node->getOp().column);
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generateUnaryExprFloat(const std::shared_ptr<UnaryExpr> &node,
                                                    llvm::Value *rhs) {
  switch (node->getOp().tokenType) {
  case ADD:
    return rhs; // if +Expr is the same as Expr
  case SUB:
    return builder->CreateFNeg(rhs);
  default:
    reportError("Internal Error - Unary Operator not allowed for float type", node->getOp().line,
                node->getOp().column);
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generateUnaryExprBool(const std::shared_ptr<UnaryExpr> &node,
                                                   llvm::Value *rhs) {
  switch (node->getOp().tokenType) {
  case NOT:
    return builder->CreateNot(rhs, "nottemp");
  default:
    reportError("Internal Error - Unary Operator not allowed for bool type", node->getOp().line,
                node->getOp().column);
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generate(const std::shared_ptr<UnaryExpr> &node) {
  llvm::Value *rhs = CodeGeneration::generate(node->getRhs());

  // To decide the type, we use the child's type because that is the type of the operand
  // The type of the node is the type after applying the unary operator
  switch (node->getRhs()->getType()->getTypeKind()) {
  case Type::Kind::BasicType: {
    auto type = std::dynamic_pointer_cast<BasicType>(node->getType());
    switch (type->getKind()) {
    case BasicType::Kind::INT:
      return generateUnaryExprInt(node, rhs);
    case BasicType::Kind::FLOAT:
      return generateUnaryExprFloat(node, rhs);
    case BasicType::Kind::BOOL:
      return generateUnaryExprBool(node, rhs);
    case BasicType::Kind::STRING:
      reportError("Internal Error - Binary Operator not allowed for string type",
                  node->getOp().line, node->getOp().column);
    default:
      reportError("Internal Error - Binar Operator not allowed for type not known",
                  node->getOp().line, node->getOp().column);
      return nullptr;
    }
  }
  case Type::Kind::Signature:
    reportError("Internal Error - Binary Operator can not be of type function", node->getOp().line,
                node->getOp().column);
    return nullptr;
  default:
    reportError("Internal Error - Binary Operator of type not known", node->getOp().line,
                node->getOp().column);
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generate(const std::shared_ptr<LiteralExpr> &node) {
  if (node->getType()->getTypeKind() == Type::Kind::BasicType) {
    auto type = std::dynamic_pointer_cast<BasicType>(node->getType());
    switch (type->getKind()) {
    case BasicType::Kind::INT: {
      int v = std::stoi(node->getToken().value);
      return llvm::ConstantInt::get(builder->getInt64Ty(), v);
    }
    case BasicType::Kind::FLOAT: {
      double v = std::stod(node->getToken().value);
      return llvm::ConstantFP::get(builder->getDoubleTy(), v);
    }
    case BasicType::Kind::BOOL: {
      int v = node->getToken().value == "true" ? 1 : 0;
      return llvm::ConstantInt::get(builder->getInt1Ty(), v);
    }
    case BasicType::Kind::STRING: {
      std::string v = node->getToken().value;
      auto ty = llvm::ArrayType::get(llvm::Type::getInt8Ty(context),
                                     v.size() + 1); // because null terminator
      auto index0 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
      llvm::Constant *constant = llvm::ConstantDataArray::getString(context, v, true);
      auto *var =
          new llvm::GlobalVariable(*module, ty, true, llvm::GlobalValue::InternalLinkage, constant);

      return builder->CreateGEP(var, {index0, index0});
    }
    default:
      reportError("Internal Error - Literal Expr with basic type not known", node->getToken().line,
                  node->getToken().column);
      return nullptr;
    }
  } else {
    reportError("Internal Error - Literal Expr can not be of type function", node->getToken().line,
                node->getToken().column);
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generate(const std::shared_ptr<IdentExpr> &node) {
  auto localvariable = localVariables.find(node->getName());
  if (localvariable != localVariables.end()) {
    llvm::Value *v =
        builder->CreateLoad(getLLVMType(node->getType()), localvariable->second, "tempload");
    return v;
  } else {
    // Variable is not local, check if it is global
    auto globalvariable = globalVariables.find(node->getName());
    if (globalvariable != globalVariables.end()) {
      llvm::Value *v = builder->CreateLoad(getLLVMType(node->getType()), globalvariable->second);
      return v;
    } else {
      reportError("Internal Error - Variable referenced does not exist", node->getIdent().line,
                  node->getIdent().column);
      return nullptr;
    }
  }
}

bool CodeGeneration::isBuiltinFunction(std::string functionName) {
  auto found = builtinFunctions.find(functionName);
  return found != builtinFunctions.end();
}

llvm::Value *CodeGeneration::generateCallBuiltinFunction(std::string functionName,
                                                         const std::shared_ptr<CallExpr> &node) {
  if (functionName == "print") {
    return generateCallPrint(node);
  } else if (functionName == "println") {
    return generateCallPrintln(node);
  } else {
    reportError("Internal Error - Builtin function " + functionName + " does not exist");
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generateCallPrint(const std::shared_ptr<CallExpr> &node) {
  std::vector<llvm::Value *> args;

  // Choose which format are we going to print it depending on the type of the argument
  if (node->getArgs().size() > 1) {
    reportError("Internal Error - Must call print with 1 argument");
    return nullptr;
  }

  if (node->getArgs()[0]->getType()->getTypeKind() == Type::Kind::BasicType) {
    auto type = std::dynamic_pointer_cast<BasicType>(node->getArgs()[0]->getType());
    // size = 3 -> %_\00
    auto tyOfStringFormat = llvm::ArrayType::get(llvm::Type::getInt8Ty(context), 3);
    llvm::Value *gvar;

    switch (type->getKind()) {
    case BasicType::Kind::INT: {
      llvm::Constant *constant = llvm::ConstantDataArray::getString(context, "%d", true);
      gvar = new llvm::GlobalVariable(*module, tyOfStringFormat, true,
                                      llvm::GlobalValue::InternalLinkage, constant);
      break;
    }
    case BasicType::Kind::FLOAT: {
      llvm::Constant *constant = llvm::ConstantDataArray::getString(context, "%f", true);
      gvar = new llvm::GlobalVariable(*module, tyOfStringFormat, true,
                                      llvm::GlobalValue::InternalLinkage, constant);
      break;
    }
    case BasicType::Kind::BOOL: {
      llvm::Constant *constant = llvm::ConstantDataArray::getString(context, "%s", true);
      gvar = new llvm::GlobalVariable(*module, tyOfStringFormat, true,
                                      llvm::GlobalValue::InternalLinkage, constant);
      break;
    }
    case BasicType::Kind::STRING: {
      llvm::Constant *constant = llvm::ConstantDataArray::getString(context, "%s", true);
      gvar = new llvm::GlobalVariable(*module, tyOfStringFormat, true,
                                      llvm::GlobalValue::InternalLinkage, constant);
      break;
    }
    default:
      reportError("Internal Error - Unknown basic type for builtin function print");
      return nullptr;
    }

    auto index0 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    auto *gepInst = builder->CreateGEP(gvar, {index0, index0});
    args.push_back(gepInst);

    if (node->getArgs().size() > 1) {
      reportError("Internal Error - Can not call print with more than 1 argument");
      return nullptr;
    }

    // All the basic data types can be printed directly except bool, so we have to treat it
    if (type->isBoolean()) {
      // TODO: move constants true and false outside so we dont define them every time
      // We define the string representation of true and false
      auto tyOfStringTrue = llvm::ArrayType::get(llvm::Type::getInt8Ty(context), 5);
      auto tyOfStringFalse = llvm::ArrayType::get(llvm::Type::getInt8Ty(context), 6);
      llvm::Constant *constantTrue = llvm::ConstantDataArray::getString(context, "true", true);
      llvm::Constant *constantFalse = llvm::ConstantDataArray::getString(context, "false", true);
      auto *varTrue = new llvm::GlobalVariable(*module, tyOfStringTrue, true,
                                               llvm::GlobalValue::InternalLinkage, constantTrue);
      auto *gepTrue = builder->CreateGEP(varTrue, {index0, index0});
      auto *varFalse = new llvm::GlobalVariable(*module, tyOfStringFalse, true,
                                                llvm::GlobalValue::InternalLinkage, constantFalse);
      auto *gepFalse = builder->CreateGEP(varFalse, {index0, index0});
      auto arg = generate(node->getArgs()[0]);
      // Now we have to compare it to check if it is false or true
      auto *cmpInst =
          builder->CreateICmpNE(arg, llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), 0));
      auto *selectInst = builder->CreateSelect(cmpInst, gepTrue, gepFalse);
      args.push_back(selectInst);
    } else {
      args.push_back(generate(node->getArgs()[0]));
    }

    // The function that we are really calling is "printf" from the std library of c
    llvm::Function *callee = module->getFunction("printf");
    return builder->CreateCall(callee, args, "calltmp");
  } else if (node->getType()->getTypeKind() == Type::Kind::Signature) {
    reportError("Internal Error - Signature type for builtin function print");
    return nullptr;
  } else {
    reportError("Internal Error - Unknown type for builtin function print");
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generateCallPrintln(const std::shared_ptr<CallExpr> &node) {
  std::vector<llvm::Value *> args;

  // Choose which format are we going to print it depending on the type of the argument
  if (node->getArgs().size() > 1) {
    reportError("Internal Error - Must call print with 1 argument");
    return nullptr;
  }

  if (node->getArgs()[0]->getType()->getTypeKind() == Type::Kind::BasicType) {
    auto type = std::dynamic_pointer_cast<BasicType>(node->getArgs()[0]->getType());
    // size = 4 -> %_\n\00
    auto tyOfStringFormat = llvm::ArrayType::get(llvm::Type::getInt8Ty(context), 4);
    llvm::Value *gvar;

    switch (type->getKind()) {
    case BasicType::Kind::INT: {
      llvm::Constant *constant = llvm::ConstantDataArray::getString(context, "%d\n", true);
      gvar = new llvm::GlobalVariable(*module, tyOfStringFormat, true,
                                      llvm::GlobalValue::InternalLinkage, constant);
      break;
    }
    case BasicType::Kind::FLOAT: {
      llvm::Constant *constant = llvm::ConstantDataArray::getString(context, "%f\n", true);
      gvar = new llvm::GlobalVariable(*module, tyOfStringFormat, true,
                                      llvm::GlobalValue::InternalLinkage, constant);
      break;
    }
    case BasicType::Kind::BOOL: {
      llvm::Constant *constant = llvm::ConstantDataArray::getString(context, "%s\n", true);
      gvar = new llvm::GlobalVariable(*module, tyOfStringFormat, true,
                                      llvm::GlobalValue::InternalLinkage, constant);
      break;
    }
    case BasicType::Kind::STRING: {
      llvm::Constant *constant = llvm::ConstantDataArray::getString(context, "%s\n", true);
      gvar = new llvm::GlobalVariable(*module, tyOfStringFormat, true,
                                      llvm::GlobalValue::InternalLinkage, constant);
      break;
    }
    default:
      reportError("Internal Error - Unknown basic type for builtin function print");
      return nullptr;
    }

    auto index0 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    auto *gepInst = builder->CreateGEP(gvar, {index0, index0});
    args.push_back(gepInst);

    if (node->getArgs().size() > 1) {
      reportError("Internal Error - Can not call print with more than 1 argument");
      return nullptr;
    }

    // All the basic data types can be printed directly except bool, so we have to treat it
    if (type->isBoolean()) {
      // TODO: move constants true and false outside so we dont define them every time
      // We define the string representation of true and false
      auto tyOfStringTrue = llvm::ArrayType::get(llvm::Type::getInt8Ty(context), 5);
      auto tyOfStringFalse = llvm::ArrayType::get(llvm::Type::getInt8Ty(context), 6);
      llvm::Constant *constantTrue = llvm::ConstantDataArray::getString(context, "true", true);
      llvm::Constant *constantFalse = llvm::ConstantDataArray::getString(context, "false", true);
      auto *varTrue = new llvm::GlobalVariable(*module, tyOfStringTrue, true,
                                               llvm::GlobalValue::InternalLinkage, constantTrue);
      auto *gepTrue = builder->CreateGEP(varTrue, {index0, index0});
      auto *varFalse = new llvm::GlobalVariable(*module, tyOfStringFalse, true,
                                                llvm::GlobalValue::InternalLinkage, constantFalse);
      auto *gepFalse = builder->CreateGEP(varFalse, {index0, index0});
      auto arg = generate(node->getArgs()[0]);
      // Now we have to compare it to check if it is false or true
      auto *cmpInst =
          builder->CreateICmpNE(arg, llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), 0));
      auto *selectInst = builder->CreateSelect(cmpInst, gepTrue, gepFalse);
      args.push_back(selectInst);
    } else {
      args.push_back(generate(node->getArgs()[0]));
    }

    // The function that we are really calling is "printf" from the std library of c
    llvm::Function *callee = module->getFunction("printf");
    return builder->CreateCall(callee, args, "calltmp");
  } else if (node->getType()->getTypeKind() == Type::Kind::Signature) {
    reportError("Internal Error - Signature type for builtin function print");
    return nullptr;
  } else {
    reportError("Internal Error - Unknown type for builtin function print");
    return nullptr;
  }
}

llvm::Value *CodeGeneration::generate(const std::shared_ptr<CallExpr> &node) {
  std::string functionName = getIdentifier(node->getFunc());

  if (isBuiltinFunction(functionName)) {
    return generateCallBuiltinFunction(functionName, node);
  } else {
    llvm::Function *callee = module->getFunction(functionName);

    if (callee == nullptr) {
      reportError("Internal Error - Function name for call was not found");
      return nullptr;
    }

    std::vector<llvm::Value *> args;
    for (const auto &arg : node->getArgs()) {
      args.push_back(generate(arg));
    }

    return builder->CreateCall(callee, args);
  }
}