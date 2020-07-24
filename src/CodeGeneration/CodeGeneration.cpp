#include "stoc/CodeGeneration/CodeGeneration.h"

#include <llvm/IR/Verifier.h>
#include <llvm/Bitcode/BitcodeWriter.h>

CodeGeneration::CodeGeneration(std::shared_ptr<SrcFile> &file) : file(file) {
  module = std::make_shared<llvm::Module>(file->getFilename(), this->context);
  builder = std::make_shared<llvm::IRBuilder<>>(context);
  // TODO: see how to module.setDataLayout()
}

void CodeGeneration::generate() {
  for (const auto &declaration : file->getAst()) {
    generate(declaration);
  }

  llvm::verifyModule(*module, &llvm::errs(), nullptr);

  std::cout << std::endl;
  std::cout << std::endl;
  module->print(llvm::errs(), nullptr);
}

llvm::Type *CodeGeneration::getLLVMType(std::string type) {
  if (type == "int") {
    return llvm::Type::getInt64Ty(context);
  } else if (type == "float") {
    return llvm::Type::getDoubleTy(context);
  } else if (type == "bool") {
    return llvm::Type::getInt1Ty(context);
  }

  // TODO: add strings handler
  // else if(type == "string") { return builder->get}
}

std::string CodeGeneration::getIdentifier(const std::shared_ptr<Expr> &node) {
  std::shared_ptr<IdentExpr> identExpr = std::dynamic_pointer_cast<IdentExpr>(node);

  // make sure identExpr is not null
  if (identExpr != nullptr) {
    return identExpr->getName();
  }

  // TODO: check what happens if dynamic casting does not succeed
}