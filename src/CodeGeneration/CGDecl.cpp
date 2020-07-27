# TODO: add header
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

void CodeGeneration::generate(const std::shared_ptr<VarDecl> &node) {
  if (node->isGlobal()) {
    llvm::Constant *constant0 = llvm::ConstantInt::get(this->builder->getInt64Ty(), 0);
    auto *GV = new llvm::GlobalVariable(*module, getLLVMType(node->getTypeToken().value), false,
                                        llvm::GlobalValue::PrivateLinkage, constant0,
                                        node->getIdentifierToken().value, nullptr);
    globalVariables[node->getIdentifierToken().value] = GV;
    // Because global variable declarations might have a complex initialization (not just a simple
    // constant expression):
    // var int a = 5 + 4 * 3 - 2;
    // var int b = 10 + a;
    // and constant folding is not implemented, it is necessary to build a function that initializes
    // every global variable
    generateFunctionForInitialization(node, GV);
  } else {
    // For local variables:
    // Variable is defined
    // TODO: improvement: put alloca in the entry block of the function (see:
    //       http://lists.llvm.org/pipermail/llvm-dev/2017-January/108730.html)
    auto *allocaInst = builder->CreateAlloca(getLLVMType(node->getTypeToken().value), nullptr,
                                             node->getIdentifierToken().value);
    // Generate code to calculate the initializer value
    llvm::Value *value = generate(node->getValue());
    // Store the initializer value in the variable
    builder->CreateStore(value, allocaInst);
    // The reference to the variable is stored to access it later
    localVariables[node->getIdentifierToken().value] = allocaInst;
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

void CodeGeneration::generate(const std::shared_ptr<ConstDecl> &node) {
  if (node->isGlobal()) {
    auto *GV = new llvm::GlobalVariable(*module, getLLVMType(node->getTypeToken().value), true,
                                        llvm::GlobalValue::PrivateLinkage, nullptr,
                                        node->getIdentifierToken().value, nullptr);
    globalVariables[node->getIdentifierToken().value] = GV;
    // Because global variable declarations might have a complex initialization (not just a simple
    // constant expression):
    // var int a = 5 + 4 * 3 - 2;
    // var int b = 10 + a;
    // and constant folding is not implemented, it is necessary to build a function that initializes
    // every global variable
    generateFunctionForInitialization(node, GV);
  } else {
    // For local variables:
    // Variable is defined
    // TODO: improvement: put alloca in the entry block of the function (see:
    //       http://lists.llvm.org/pipermail/llvm-dev/2017-January/108730.html)
    auto *allocaInst = builder->CreateAlloca(getLLVMType(node->getTypeToken().value), nullptr,
                                             node->getIdentifierToken().value);
    // Generate code to calculate the initializer value
    llvm::Value *value = generate(node->getValue());
    // Store the initializer value in the variable
    builder->CreateStore(value, allocaInst);
    // The reference to the variable is stored to access it later
    localVariables[node->getIdentifierToken().value] = allocaInst;
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
    params.push_back(getLLVMType(param->getTypeToken().value));
  }

  // 1.2 Return Type and Parameters
  // TODO: handle void type in return
  llvm::Type *returnType = getLLVMType(node->getReturnTypeToken().value);
  llvm::FunctionType *functionType = llvm::FunctionType::get(returnType, params, false);

  // 2. Create Function
  llvm::Function *function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage,
                                                    node->getIdentifierToken().value, module.get());

  // 2.1 Set name for all parameters
  int idx = 0;
  for (auto &arg : function->args()) {
    arg.setName(node->getParams()[idx]->getIdentifierToken().value);
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
    localVariables[param->getIdentifierToken().value] =
        builder->CreateAlloca(getLLVMType(param->getTypeToken().value), nullptr);
  }

  // If there is return type, there will be return statement so we create the special variable
  if (node->isHasReturnType()) {
    localVariables["return"] =
        builder->CreateAlloca(getLLVMType(node->getReturnTypeToken().value), nullptr, "return");
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
    auto load = builder->CreateLoad(getLLVMType(node->getReturnTypeToken().value),
                                    localVariables["return"]);
    builder->CreateRet(load);
  }
}