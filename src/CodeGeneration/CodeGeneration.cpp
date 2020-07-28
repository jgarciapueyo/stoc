#include "stoc/CodeGeneration/CodeGeneration.h"

#include <llvm/IR/Verifier.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Program.h>

CodeGeneration::CodeGeneration(std::shared_ptr<SrcFile> &file) : file(file) {
  module = std::make_shared<llvm::Module>(file->getFilename(), this->context);
  builder = std::make_shared<llvm::IRBuilder<>>(context);
  initialization();
}

void CodeGeneration::initialization() {
  // Initialize the target registry etc.
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  auto TargetTriple = llvm::sys::getDefaultTargetTriple();
  module->setTargetTriple(TargetTriple);

  std::string Error;
  auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

  // Print an error and exit if we couldn't find the requested target.
  // This generally occurs if we've forgotten to initialise the
  // TargetRegistry or we have a bogus target triple.
  if (!Target) {
    llvm::errs() << Error;
  }

  auto CPU = "generic";
  auto Features = "";

  llvm::TargetOptions opt;
  auto RM = llvm::Optional<llvm::Reloc::Model>();
  auto TheTargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

  module->setDataLayout(TheTargetMachine->createDataLayout());
}

void CodeGeneration::generate() {
  for (const auto &declaration : file->getAst()) {
    generate(declaration);
  }

  llvm::verifyModule(*module, &llvm::errs(), nullptr);

  //std::cout << std::endl;
  //std::cout << std::endl;
  module->print(llvm::errs(), nullptr);
  getExecutable();
}

void CodeGeneration::getExecutable() {
  auto Filename = "output.bc";
  std::error_code EC;
  llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);

  if (EC) {
    llvm::errs() << "Could not open file: " << EC.message();
  }

  llvm::WriteBitcodeToFile(*module, dest);
  dest.close();

  std::string ec;
  llvm::ErrorOr<std::string> llc = llvm::sys::findProgramByName("llc");
  std::cout << llc.get() << std::endl;
  int resultcode = llvm::sys::ExecuteAndWait(llc.get(), {"llc", "output.bc", "-filetype=obj", "-addrsig"}, llvm::None, {}, 1, 0, &ec);
  if(resultcode != 0) {
    llvm::errs() << ec;
  }

  llvm::ErrorOr<std::string> gcc = llvm::sys::findProgramByName("gcc");
  std::cout << gcc.get() << std::endl;
  resultcode = llvm::sys::ExecuteAndWait(gcc.get(), {"gcc", "-no-pie", "output.o"}, llvm::None, {}, 1, 0, &ec);
  if(resultcode != 0) {
    llvm::errs() << ec;
  }
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