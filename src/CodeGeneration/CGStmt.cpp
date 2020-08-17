//=== src/CodeGeneration/CGStmt.cpp - Impl of CodeGeneration for Statements ---------*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements the methods of the CodeGeneration class used to generate code
// for Statements.
//
//===------------------------------------------------------------------------------------------===//
#include "stoc/CodeGeneration/CodeGeneration.h"

void CodeGeneration::generate(const std::shared_ptr<Stmt> &node) {
  switch (node->getStmtKind()) {
  case Stmt::Kind::DECLARATIONSTMT:
    return generate(std::static_pointer_cast<DeclarationStmt>(node));
  case Stmt::Kind::EXPRESSIONSTMT:
    return generate(std::static_pointer_cast<ExpressionStmt>(node));
  case Stmt::Kind::BLOCKSTMT:
    return generate(std::static_pointer_cast<BlockStmt>(node));
  case Stmt::Kind::IFSTMT:
    return generate(std::static_pointer_cast<IfStmt>(node));
  case Stmt::Kind::FORSTMT:
    return generate(std::static_pointer_cast<ForStmt>(node));
  case Stmt::Kind::WHILESTMT:
    return generate(std::static_pointer_cast<WhileStmt>(node));
  case Stmt::Kind::ASSIGNMENTSTMT:
    return generate(std::static_pointer_cast<AssignmentStmt>(node));
  case Stmt::Kind::RETURNSTMT:
    return generate(std::static_pointer_cast<ReturnStmt>(node));
  }
}

void CodeGeneration::generate(const std::shared_ptr<DeclarationStmt> &node) {
  generate(node->getDecl());
}

void CodeGeneration::generate(const std::shared_ptr<ExpressionStmt> &node) {
  generate(node->getExpr());
}

void CodeGeneration::generate(const std::shared_ptr<BlockStmt> &node) {
  // IMPORTANT: the llvm notion of basic block where the stmts will be generated has to be defined
  //           before calling this method

  for (const auto &stmt : node->getStmts()) {
    generate(stmt);
  }
};

void CodeGeneration::generate(const std::shared_ptr<IfStmt> &node) {
  // Get current function where the code is being generated
  llvm::Function *function = builder->GetInsertBlock()->getParent();

  // Code Generation for the condition of the ifStmt
  llvm::Value *condition = generate(node->getCondition());

  if (node->isHasElse()) {
    // Create Basic Blocks for the then and else cases
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then");
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(context, "else");
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "continuationif");

    // Create branching depending on condition
    builder->CreateCondBr(condition, thenBB, elseBB);

    // Code generation for the then branch
    function->getBasicBlockList().push_back(thenBB);
    builder->SetInsertPoint(thenBB);
    generate(node->getThenBranch());

    // If this BasicBlock has not been terminated (i.e with a return statement), an inconditional
    // branch to the continuation of the ifStmt is added
    if (!thenBB->getTerminator()) {
      builder->CreateBr(mergeBB);
    }

    // In addition to that block, we look at the current block we are inserting because maybe the
    // node->getThenBranch() has generated other basic blocks
    if (!builder->GetInsertBlock()->getTerminator()) {
      builder->CreateBr(mergeBB);
    }

    // Code generation for the else branch
    function->getBasicBlockList().push_back(elseBB);
    builder->SetInsertPoint(elseBB);
    generate(node->getElseBranch());

    // If this BasicBlock has not been terminated (i.e with a return statement), an inconditional
    // branch to the continuation of the ifStmt is added
    if (!elseBB->getTerminator()) {
      builder->CreateBr(mergeBB);
    }

    // In addition to that block, we look at the current block we are inserting because maybe the
    // node->getElseBranch() has generated other basic blocks
    if (!builder->GetInsertBlock()->getTerminator()) {
      builder->CreateBr(mergeBB);
    }

    // Code generation for the merge basic block
    function->getBasicBlockList().push_back(mergeBB);
    builder->SetInsertPoint(mergeBB);
  } else {
    // Create Basic Blocks for the then and else cases
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then");
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "continuationif");

    // Create branching depending on condition
    builder->CreateCondBr(condition, thenBB, mergeBB);

    // Code generation for the then branch
    function->getBasicBlockList().push_back(thenBB);
    builder->SetInsertPoint(thenBB);
    generate(node->getThenBranch());

    // If this BasicBlock has not been terminated (i.e with a return statement), an inconditional
    // branch to the continuation of the ifStmt is added
    if (!thenBB->getTerminator()) {
      builder->CreateBr(mergeBB);
    }

    // Code generation for the merge basic block
    function->getBasicBlockList().push_back(mergeBB);
    builder->SetInsertPoint(mergeBB);
  }
}

void CodeGeneration::generate(const std::shared_ptr<ForStmt> &node) {
  // Get current function where the code is being generated
  llvm::Function *function = builder->GetInsertBlock()->getParent();

  // Create Basic Block for condition, body and post
  llvm::BasicBlock *conditionBB = llvm::BasicBlock::Create(context, "conditionfor");
  llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(context, "bodyfor");
  llvm::BasicBlock *postBB = llvm::BasicBlock::Create(context, "postfor");
  llvm::BasicBlock *continuationBB = llvm::BasicBlock::Create(context, "continuationfor");

  // Code Generation for the initialization
  generate(node->getInit());

  // Code Generation for the condition of the forstmt
  builder->CreateBr(conditionBB);
  function->getBasicBlockList().push_back(conditionBB);
  builder->SetInsertPoint(conditionBB);
  llvm::Value *condition = generate(node->getCond());
  builder->CreateCondBr(condition, bodyBB, continuationBB);

  // Code Generation for the body of the forstmt
  function->getBasicBlockList().push_back(bodyBB);
  builder->SetInsertPoint(bodyBB);
  generate(node->getBody());
  // If this BasicBlock has not been terminated (i.e with a return statement), an inconditional
  // branch to the continuation of the ifStmt is added
  if(!bodyBB->getTerminator()) {
    builder->CreateBr(postBB);
  }

  // Code Generation for the post of the forstmt
  function->getBasicBlockList().push_back(postBB);
  builder->SetInsertPoint(postBB);
  generate(node->getPost());
  builder->CreateBr(conditionBB);

  // Code Generation for the continuation of the for stmt
  function->getBasicBlockList().push_back(continuationBB);
  builder->SetInsertPoint(continuationBB);
}

void CodeGeneration::generate(const std::shared_ptr<WhileStmt> &node) {
  // Get current function where the code is being generated
  llvm::Function *function = builder->GetInsertBlock()->getParent();

  // Create Basic Block for the condition, body and continuation
  llvm::BasicBlock *conditionBB = llvm::BasicBlock::Create(context, "conditionwhile");
  llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(context, "bodywhile");
  llvm::BasicBlock *continuationBB = llvm::BasicBlock::Create(context, "continuationwhile");

  // Code Generation for the condition of the whileStmt
  builder->CreateBr(conditionBB);
  function->getBasicBlockList().push_back(conditionBB);
  builder->SetInsertPoint(conditionBB);
  llvm::Value *condition = generate(node->getCond());
  builder->CreateCondBr(condition, bodyBB, continuationBB);

  // Code Generation for the body
  function->getBasicBlockList().push_back(bodyBB);
  builder->SetInsertPoint(bodyBB);
  generate(node->getBody());
  // If this BasicBlock has not been terminated (i.e with a return statement), an inconditional
  // branch to the continuation of the ifStmt is added
  if(!bodyBB->getTerminator()) {
    builder->CreateBr(conditionBB);
  }

  // Code Generation for the continuation of the while stmt
  function->getBasicBlockList().push_back(continuationBB);
  builder->SetInsertPoint(continuationBB);
}

void CodeGeneration::generate(const std::shared_ptr<AssignmentStmt> &node) {
  llvm::Value *rhs = generate(node->getRhs());
  std::string lhsName = getIdentifier(node->getLhs());
  // TODO: refactor how to look for lhs name (maybe its not a IdentExpr)
  auto lhslocal = localVariables.find(lhsName);
  if (lhslocal != localVariables.end()) {
    // assignment to local variable
    builder->CreateStore(rhs, lhslocal->second);
  } else {
    // LHS is not local variable, check if it is global
    auto lhsglobal = globalVariables.find(lhsName);
    if (lhsglobal != globalVariables.end()) {
      builder->CreateStore(rhs, lhsglobal->second);
    } else {
      // TODO: handle what happens if lhs does not exist
    }
  }
}

void CodeGeneration::generate(const std::shared_ptr<ReturnStmt> &node) {
  llvm::Value *ret = generate(node->getValue());

  // Instead of return the value, we store it in the special variable "return" and jump to exit
  // basic block
  builder->CreateStore(ret, localVariables["return"]);
  builder->CreateBr(this->exitBB);
}