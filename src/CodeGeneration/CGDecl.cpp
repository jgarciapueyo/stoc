//=== src/CodeGeneration/CGDecl.cpp - Impl of CodeGeneration for Declarations -------*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements the methods of the CodeGeneration class used to generate code
// for Declarations.
//
//===------------------------------------------------------------------------------------------===//
#include "stoc/CodeGeneration/CodeGeneration.h"

#include <llvm/Transforms/Utils/ModuleUtils.h>

void CodeGeneration::generate(const std::shared_ptr<Decl> &node) {
  switch (node->getDeclKind()) {
  case Decl::Kind::VARDECL:
    return generate(std::static_pointer_cast<VarDecl>(node));
  case Decl::Kind::CONSTDECL:
    return generate(std::static_pointer_cast<ConstDecl>(node));
  case Decl::Kind::PARAMDECL:
    return generate(std::static_pointer_cast<ParamDecl>(node));
  case Decl::Kind::FUNCDECL:
    return generate(std::static_pointer_cast<FuncDecl>(node));
  }
}

void CodeGeneration::generateFunctionForInitialization(const std::shared_ptr<VarDecl> &node,
                                                       llvm::Value *GV) {
  // Define function signature
  // 1. No parameters
  std::vector<llvm::Type *> params;
  // 2. Return Type and Parameters
  llvm::Type *returnType = llvm::Type::getVoidTy(context);
  llvm::FunctionType *functionType = llvm::FunctionType::get(returnType, params, false);
  // 3. Create Function
  llvm::Function *function = llvm::Function::Create(functionType, llvm::Function::InternalLinkage,
                                                    "_global_var_init", module.get());
  // 4. Create basic block for the function
  llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(context, "entry", function);
  builder->SetInsertPoint(entryBB);

  // 5. Code Generation for the initialization
  llvm::Value *initializer = generate(node->getValue());
  builder->CreateStore(initializer, GV);
  builder->CreateRetVoid();

  // 6. Add initializer function
  llvm::appendToGlobalCtors(*module, function, 0, nullptr);

  // NOTE that could be implemented to improve similarity to Clang: to make CodeGen more similar to
  // clang, every constructor function is called by a general constructor function
  // (_GLOBAL__modulename.st()) and only that general constructor function is appended to
  // llvm.global_ctors
}

void CodeGeneration::generateGlobalVariableDecl(const std::shared_ptr<VarDecl> &node) {
  llvm::Type *LLVMtype = getLLVMType(node->getType());
  llvm::Constant *constant0 = getLLVMInit(node->getType());
  auto *GV = new llvm::GlobalVariable(*module, LLVMtype, false, llvm::GlobalValue::PrivateLinkage,
                                      constant0, node->getIdentifierMangled(), nullptr);

  globalVariables[node->getIdentifierMangled()] = GV;
  // Because global variable declarations might have a complex initialization (not just a simple
  // constant expression):
  // var int a = 5 + 4 * 3 - 2;
  // var int b = 10 + a;
  // and constant folding is not implemented, it is necessary to build a function that initializes
  // every global variable
  generateFunctionForInitialization(node, GV);
}

void CodeGeneration::generateLocalVariableDecl(const std::shared_ptr<VarDecl> &node) {
  llvm::Type *LLVMtype = getLLVMType(node->getType());
  // TODO: (improvement) put alloca in the entry block of the function (see:
  //       http://lists.llvm.org/pipermail/llvm-dev/2017-January/108730.html)
  auto *allocaInst = builder->CreateAlloca(LLVMtype, nullptr, node->getIdentifierMangled());
  // Generate code to calculate the initializer value
  llvm::Value *value = generate(node->getValue());
  // Store the initializer value in the variable
  builder->CreateStore(value, allocaInst);
  // The reference to the variable is stored to access it later
  localVariables[node->getIdentifierMangled()] = allocaInst;
}

void CodeGeneration::generate(const std::shared_ptr<VarDecl> &node) {
  if (node->isGlobal()) {
    switch (node->getType()->getTypeKind()) {
    case Type::Kind::BasicType:
      generateGlobalVariableDecl(node);
      break;
    case Type::Kind::Signature:
      reportError("Internal Error - Global Variable Declaration can not be of type function",
                  node->getVarKeywordToken().line, node->getVarKeywordToken().column);
      break;
    default:
      reportError("Internal Error - Global Variable Declaration of type not known",
                  node->getVarKeywordToken().line, node->getVarKeywordToken().column);
    }
  } else {
    switch (node->getType()->getTypeKind()) {
    case Type::Kind::BasicType:
      generateLocalVariableDecl(node);
      break;
    case Type::Kind::Signature:
      reportError("Internal Error - Local Variable Declaration can not be of type function",
                  node->getVarKeywordToken().line, node->getVarKeywordToken().column);
      break;
    default:
      reportError("Internal Error - Local Variable Declaration of type not known",
                  node->getVarKeywordToken().line, node->getVarKeywordToken().column);
    }
  }
}

void CodeGeneration::generateFunctionForInitialization(const std::shared_ptr<ConstDecl> &node,
                                                       llvm::Value *GV) {
  // Define function signature
  // 1. No parameters
  std::vector<llvm::Type *> params;
  // 2. Return Type and Parameters
  llvm::Type *returnType = llvm::Type::getVoidTy(context);
  llvm::FunctionType *functionType = llvm::FunctionType::get(returnType, params, false);
  // 3. Create Function
  llvm::Function *function = llvm::Function::Create(functionType, llvm::Function::PrivateLinkage,
                                                    "_global_var_init", module.get());
  // 4. Create basic block for the function
  llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(context, "entry", function);
  builder->SetInsertPoint(entryBB);

  // 5. Code Generation for the initialization
  auto initializer = generate(node->getValue());
  builder->CreateStore(initializer, GV);
  builder->CreateRetVoid();

  // 6. Add initializer function
  llvm::appendToGlobalCtors(*module, function, 0, nullptr);

  // NOTE that could be implemented to improve similarity to Clang: to make CodeGen more similar to
  // clang, every constructor function is called by a general constructor function
  // (_GLOBAL__modulename.st()) and only that general constructor function is appended to
  // llvm.global_ctors
}

void CodeGeneration::generateGlobalConstantDecl(const std::shared_ptr<ConstDecl> &node) {
  llvm::Type *LLVMtype = getLLVMType(node->getType());
  llvm::Constant *constant0 = getLLVMInit(node->getType());
  auto *GV = new llvm::GlobalVariable(*module, LLVMtype, true, llvm::GlobalValue::PrivateLinkage,
                                      constant0, node->getIdentifierMangled(), nullptr);

  globalVariables[node->getIdentifierMangled()] = GV;
  // Because global variable declarations might have a complex initialization (not just a simple
  // constant expression):
  // var int a = 5 + 4 * 3 - 2;
  // var int b = 10 + a;
  // and constant folding is not implemented, it is necessary to build a function that initializes
  // every global variable
  generateFunctionForInitialization(node, GV);
}

void CodeGeneration::generateLocalConstantDecl(const std::shared_ptr<ConstDecl> &node) {
  llvm::Type *LLVMtype = getLLVMType(node->getType());
  // TODO: (improvement) put alloca in the entry block of the function (see:
  //       http://lists.llvm.org/pipermail/llvm-dev/2017-January/108730.html)
  auto *allocaInst = builder->CreateAlloca(LLVMtype, nullptr, node->getIdentifierMangled());
  // Generate code to calculate the initializer value
  llvm::Value *value = generate(node->getValue());
  // Store the initializer value in the variable
  builder->CreateStore(value, allocaInst);
  // The reference to the variable is stored to access it later
  localVariables[node->getIdentifierMangled()] = allocaInst;
}

void CodeGeneration::generate(const std::shared_ptr<ConstDecl> &node) {
  if (node->isGlobal()) {
    switch (node->getType()->getTypeKind()) {
    case Type::Kind::BasicType:
      generateGlobalConstantDecl(node);
      break;
    case Type::Kind::Signature:
      reportError("Internal Error - Global Variable Declaration can not be of type function",
                  node->getConstKeywordToken().line, node->getConstKeywordToken().column);
      break;
    default:
      reportError("Internal Error - Global Variable Declaration of type not known",
                  node->getConstKeywordToken().line, node->getConstKeywordToken().column);
    }
  } else {
    switch (node->getType()->getTypeKind()) {
    case Type::Kind::BasicType:
      generateLocalConstantDecl(node);
      break;
    case Type::Kind::Signature:
      reportError("Internal Error - Local Variable Declaration can not be of type function",
                  node->getConstKeywordToken().line, node->getConstKeywordToken().column);
      break;
    default:
      reportError("Internal Error - Local Variable Declaration of type not known",
                  node->getConstKeywordToken().line, node->getConstKeywordToken().column);
    }
  }
}

void CodeGeneration::generate(const std::shared_ptr<ParamDecl> &node) {
  // Code Generation for parameters is handled in the method for FuncDecl
}

void CodeGeneration::generate(const std::shared_ptr<FuncDecl> &node) {
  // 1. Define function signature
  // 1.1 Parameters
  std::vector<llvm::Type *> params;

  for (const auto &param : node->getParams()) {
    params.push_back(getLLVMType(param->getType()));
  }

  // 1.2 Return Type and Parameters
  llvm::Type *returnType;
  if(node->isHasReturnType()) {
    returnType =
        getLLVMType(std::dynamic_pointer_cast<FunctionType>(node->getType())->getResult());
  } else {
    returnType = llvm::Type::getVoidTy(context);
  }

  llvm::FunctionType *functionType = llvm::FunctionType::get(returnType, params, false);

  // 2. Create Function
  llvm::Function *function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage,
                                                    node->getIdentifierMangled(), module.get());

  // 2.1 Set name for all parameters
  int idx = 0;
  for (auto &arg : function->args()) {
    arg.setName(node->getParams()[idx]->getIdentifierMangled());
    idx++;
  }

  // 3. Create basic blocks for the function
  llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(context, "entry", function);
  builder->SetInsertPoint(entryBB);

  // If function has return type, add basic block "exit" at the end. Every time there is a return
  // statement, it will store the return value to a special variable "return" and jump
  // unconditionally to the "exit" block where it will load the special variable and return it.
  // Like this, there will be only one return statement in the LLVM IR even if there are multiple
  // return statements in source code
  if (node->isHasReturnType()) {
    this->exitBB = llvm::BasicBlock::Create(context, "exit");
  }

  // 4. Insert parameters in localVariables map
  // To do this, we generate local variables (alloca instructions) and store the value of the
  // parameters in the local variables
  localVariables.clear();
  for (const auto &param : node->getParams()) {
    localVariables[param->getIdentifierMangled()] =
        builder->CreateAlloca(getLLVMType(param->getType()), nullptr);
  }

  // If there is return type, there will be return statement so we create the special variable
  if (node->isHasReturnType()) {
    localVariables["return"] = builder->CreateAlloca(
        getLLVMType(std::dynamic_pointer_cast<FunctionType>(node->getType())->getResult()), nullptr,
        "return");
  }

  for (auto &arg : function->args()) {
    builder->CreateStore(&arg, localVariables[arg.getName()]);
  }

  // 5. Code generation for body of the function
  // If we want to check if variable already exists:
  // https://prereleases.llvm.org/6.0.0/rc2/docs/tutorial/LangImpl03.html in 3.4 Function Code
  // Generation
  generate(node->getBody());

  // 6. Return
  // Every basic block in llvm must be terminated with a control flow instruction like return or
  // branch. For this reason, if the function has void return type we have to state it explicitly
  if (!node->isHasReturnType()) {
    builder->CreateRetVoid();
  } else {
    // If function has return type, it is generated:
    // Get current Basic Block
    llvm::BasicBlock *currentBlock = builder->GetInsertBlock();

    // If this BasicBlock has not been terminated (i.e with a return or other branch statement), an
    // inconditional branch to the exit basic block is added
    if (!currentBlock->getTerminator()) {
      builder->CreateBr(this->exitBB);
    }

    // Code generation for the return statement of the exit block
    function->getBasicBlockList().push_back(this->exitBB);
    builder->SetInsertPoint(this->exitBB);
    auto load = builder->CreateLoad(
        getLLVMType(std::dynamic_pointer_cast<FunctionType>(node->getType())->getResult()),
        localVariables["return"]);
    builder->CreateRet(load);
  }
}