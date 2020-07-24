#include "stoc/CodeGeneration/CodeGeneration.h"

llvm::Value *CodeGeneration::generate(const std::shared_ptr<Expr> &node) {
  switch (node->getExprType()) {
  case Expr::BINARYEXPR:
    return generate(std::static_pointer_cast<BinaryExpr>(node));
  case Expr::UNARYEXPR:
    return generate(std::static_pointer_cast<UnaryExpr>(node));
  case Expr::LITERALEXPR:
    return generate(std::static_pointer_cast<LiteralExpr>(node));
  case Expr::IDENTEXPR:
    return generate(std::static_pointer_cast<IdentExpr>(node));
  case Expr::CALLEXPR:
    return generate(std::static_pointer_cast<CallExpr>(node));
  }
}

llvm::Value *CodeGeneration::generate(const std::shared_ptr<BinaryExpr> &node) {
  llvm::Value *lhs = CodeGeneration::generate(node->getLhs());
  llvm::Value *rhs = CodeGeneration::generate(node->getRhs());

  // type of operands is same for both so is enough with looking at one of them
  std::string typeOfOperands = node->getLhs()->getType();

  if (typeOfOperands == "int") {
    switch (node->getOp().type) {
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
    }
  } else if (typeOfOperands == "float") {
    switch (node->getOp().type) {
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
    }
  } else if (typeOfOperands == "bool") {
    // TODO: improvement: see how to do it for shortcircuit logical operators
    switch (node->getOp().type) {
    case LAND:
      return builder->CreateAnd(lhs, rhs, "andtemp");
    case LOR:
      return builder->CreateOr(lhs, rhs, "ortemp");
    }
  }

  // TODO: remember work with strings too
}

llvm::Value *CodeGeneration::generate(const std::shared_ptr<UnaryExpr> &node) {
  llvm::Value *rhs = CodeGeneration::generate(node->getRhs());

  if (node->getType() == "int") {
    switch (node->getOp().type) {
    case ADD:
      return rhs; // if +Expr is the same as Expr
    case SUB: {
      // TODO: check builder->CreateNeg()
      llvm::Value *constant0 = llvm::ConstantInt::get(builder->getInt64Ty(), 0);
      return builder->CreateSub(constant0, rhs, "subtemp");
    }
    }
  } else if (node->getType() == "float") {
    switch (node->getOp().type) {
    case ADD:
      return rhs; // if +Expr is the same as Expr
    case SUB: {
      // TODO: check builder->CreateFNeg()
      llvm::Value *constant0 = llvm::ConstantFP::get(builder->getDoubleTy(), 0);
      return builder->CreateFSub(constant0, rhs, "subtemp");
    }
    }
  } else if (node->getType() == "bool") {
    switch (node->getOp().type) {
    case NOT:
      return builder->CreateNot(rhs, "nottemp");
    }
  }
}

llvm::Value *CodeGeneration::generate(const std::shared_ptr<LiteralExpr> &node) {
  if (node->getType() == "int") {
    int v = std::stoi(node->getToken().value);
    return llvm::ConstantInt::get(builder->getInt64Ty(), v);
  } else if (node->getType() == "float") {
    double v = std::stod(node->getToken().value);
    return llvm::ConstantFP::get(builder->getDoubleTy(), v);
  } else if (node->getType() == "bool") {
    int v = node->getToken().value == "true" ? 1 : 0;
    return llvm::ConstantInt::get(builder->getInt1Ty(), v);
  }
  // TODO: add strings handler
  // else if(node->getType() == "string") {
  //  std::string v = node->getToken().value;
  //  return llvm::Constant
  //}
}

llvm::Value *CodeGeneration::generate(const std::shared_ptr<IdentExpr> &node) {
  auto localvariable = localVariables.find(node->getName());
  if (localvariable != localVariables.end()) {
    llvm::Value *v = builder->CreateLoad(getLLVMType(node->getType()), localvariable->second, "tempload");
    return v;
  }
  else {
    // Variable is not local, check if it is global
    auto globalvariable = globalVariables.find(node->getName());
    if(globalvariable != globalVariables.end()) {
      llvm::Value *v = builder->CreateLoad(getLLVMType(node->getType()), globalvariable->second, "tempload");
      return v;
    }
    else {
      //TODO: handle what happens if variable does not exist
      return nullptr;
    }
  }
}

llvm::Value *CodeGeneration::generate(const std::shared_ptr<CallExpr> &node) {
  std::string functionName = getIdentifier(node->getFunc());
  llvm::Function *callee = module->getFunction(functionName);
  std::vector<llvm::Value *> args;
  for (const auto &arg : node->getArgs()) {
    args.push_back(generate(arg));
  }

  return builder->CreateCall(callee, args, "calltmp");
}