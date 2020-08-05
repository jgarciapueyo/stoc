#include "stoc/CodeGeneration/CodeGeneration.h"

#include <llvm/ADT/SmallVector.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

CodeGeneration::CodeGeneration(std::shared_ptr<SrcFile> &file) : file(file) {
  module = std::make_shared<llvm::Module>(file->getFilename(), this->context);
  builder = std::make_shared<llvm::IRBuilder<>>(context);
  initialization();
  declareBuiltinFunctions();
  declareStringBuiltinFunctions();
}

void CodeGeneration::reportError(std::string error_msg) {
  std::cerr << "<" << this->file->getFilename() << "> Code Generation: " << error_msg << std::endl;
  this->file->setErrorInCodeGeneration(true);
  throw std::runtime_error("Error in Code Generation. Could not recover.");
}

void CodeGeneration::reportError(std::string error_msg, int line, int column) {
  std::cerr << "<" << this->file->getFilename() << ":l" << line << ":c" << column
            << "> Code Generation: " << error_msg << std::endl;
  this->file->setErrorInCodeGeneration(true);
  throw std::runtime_error("Error in Code Generation. Could not recover.");
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

void CodeGeneration::declareStringBuiltinFunctions() {
  auto i8ptr = llvm::Type::getInt8PtrTy(context);
  auto i64 = llvm::Type::getInt64Ty(context);
  // strcpy function to be able to assign
  // For assigning, we will not assign but just variable to strings will be pointers to literal
  // strings
  // llvm::FunctionType *functionType_strcpy = llvm::FunctionType::get(i8ptr, {i8ptr, i8ptr},
  // false); llvm::Function::Create(functionType_strcpy, llvm::Function::ExternalLinkage, "strcpy",
  // module.get());

  // strcmp function to be able to compare
  llvm::FunctionType *functionType_strcmp = llvm::FunctionType::get(i64, {i8ptr, i8ptr}, false);
  llvm::Function::Create(functionType_strcmp, llvm::Function::ExternalLinkage, "strcmp",
                         module.get());

  // strcat function to be able to compare
  // I was not able to implement concatenation with strings in code generation
  // llvm::FunctionType *functionType_strcat = llvm::FunctionType::get(i8ptr, {i8ptr, i8ptr},
  // false); llvm::Function::Create(functionType_strcmp, llvm::Function::ExternalLinkage, "strcat",
  // module.get());
}

void CodeGeneration::declareBuiltinFunctions() {
  // print builtin function in stoc
  builtinFunctions.insert("print");
  builtinFunctions.insert("println");

  // uses the printf from std library in c
  auto i8ptr = llvm::Type::getInt8PtrTy(context);
  llvm::FunctionType *functionType_printf = llvm::FunctionType::get(i8ptr, {i8ptr}, true);
  llvm::Function::Create(functionType_printf, llvm::Function::ExternalLinkage, "printf",
                         module.get());
}

void CodeGeneration::generate() {
  try {
    for (const auto &declaration : file->getAst()) {
      generate(declaration);
    }

    bool isBroken = llvm::verifyModule(*module, &llvm::errs(), nullptr);
    if(isBroken) {
      this->file->setErrorInCodeGeneration(true);
    }

  } catch (std::runtime_error &e) {
    std::cout << e.what() << std::endl;
  }
}

void CodeGeneration::printLLVM() { module->print(llvm::errs(), nullptr); }

void CodeGeneration::getExecutable() {
  // get filename of src
  std::string filename = llvm::sys::path::stem(file->getFilename());

  std::error_code EC;
  std::string tempFilenameBitcode = filename + ".temp_bitcode";
  std::string tempFilenameObject = filename + ".o";
  llvm::raw_fd_ostream dest(tempFilenameBitcode, EC);
  if (EC) {
    llvm::errs() << "Failed to create temporary file: " << EC.message();
    return;
  }
  llvm::WriteBitcodeToFile(*module, dest);
  dest.close();

  // Program to convert LLVM bitcode to object file, it already creates the file
  llvm::ErrorOr<std::string> llc = llvm::sys::findProgramByName("llc");
  if ((EC = llc.getError())) {
    llvm::errs() << EC.message();
    return;
  }
  std::string ec;
  int resultcode = llvm::sys::ExecuteAndWait(
      llc.get(),
      {"llc", tempFilenameBitcode, "-filetype=obj", "-addrsig", "-o", tempFilenameObject},
      llvm::None, {}, 0, 0, &ec);
  if (resultcode != 0) {
    llvm::errs() << ec;
  }

  // Program to convert object file to executable, it already creates the file
  llvm::ErrorOr<std::string> gcc = llvm::sys::findProgramByName("gcc");
  if ((EC = gcc.getError())) {
    llvm::errs() << EC.message();
    return;
  }
  resultcode = llvm::sys::ExecuteAndWait(
      gcc.get(), {"gcc", "-no-pie", tempFilenameObject, "-o", filename}, llvm::None, {}, 0, 0, &ec);
  if (resultcode != 0) {
    llvm::errs() << ec;
  }

  llvm::sys::fs::remove(tempFilenameBitcode);
  llvm::sys::fs::remove(tempFilenameObject);
}

llvm::Type *CodeGeneration::getLLVMType(std::shared_ptr<Type> type) {
  if (type->getTypeKind() == Type::Kind::BasicType) {
    auto basicType = std::dynamic_pointer_cast<BasicType>(type);
    switch (basicType->getKind()) {
    case BasicType::Kind::BOOL:
      return llvm::Type::getInt1Ty(context);
    case BasicType::Kind::INT:
      return llvm::Type::getInt64Ty(context);
    case BasicType::Kind::FLOAT:
      return llvm::Type::getDoubleTy(context);
    case BasicType::Kind::STRING:
      return llvm::Type::getInt8PtrTy(context);
    case BasicType::Kind::VOID:
      return llvm::Type::getVoidTy(context);
    case BasicType::Kind::INVALID:
      reportError("Internal Error - Invalid type");
      return nullptr;
    default:
      reportError("Internal Error - Basic Type not known");
      return nullptr;
    }
  } else {
    reportError("Internal Error - Type not known");
    return nullptr;
  }
}

llvm::Constant *CodeGeneration::getLLVMInit(std::shared_ptr<Type> type) {
  if (type->getTypeKind() == Type::Kind::BasicType) {
    auto basicType = std::dynamic_pointer_cast<BasicType>(type);
    switch (basicType->getKind()) {
    case BasicType::Kind::BOOL:
      return llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), 0);
    case BasicType::Kind::INT:
      return llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
    case BasicType::Kind::FLOAT:
      return llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0);
    case BasicType::Kind::STRING:
      return llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(context));
    case BasicType::Kind::VOID:
      reportError("Internal Error - Void type can not be initialized");
      return nullptr;
    case BasicType::Kind::INVALID:
      reportError("Internal Error - Invalid type can not be initialized");
      return nullptr;
    default:
      reportError("Internal Error - Basic Type not known and can not be initialized");
      return nullptr;
    }
  } else {
    reportError("Internal Error - Type not known and can not be initialized");
    return nullptr;
  }
}

std::string CodeGeneration::getIdentifier(const std::shared_ptr<Expr> &node) {
  if (node->getExprKind() == Expr::Kind::IDENTEXPR) {
    std::shared_ptr<IdentExpr> identExpr = std::dynamic_pointer_cast<IdentExpr>(node);
    // if it does not have a reference to a declaration is a builtin
    if(identExpr->getDeclOfIdentifier() == nullptr) {
      return identExpr->getName();
    }

    // If it is has a reference to a declaration it has been defined by the user in the src code
    switch(identExpr->getDeclOfIdentifier()->getDeclKind()) {
    case Decl::Kind::CONSTDECL:
      return std::dynamic_pointer_cast<ConstDecl>(identExpr->getDeclOfIdentifier())->getIdentifierMangled();
    case Decl::Kind::VARDECL:
      return std::dynamic_pointer_cast<VarDecl>(identExpr->getDeclOfIdentifier())->getIdentifierMangled();
    case Decl::Kind::PARAMDECL:
      return std::dynamic_pointer_cast<ParamDecl>(identExpr->getDeclOfIdentifier())->getIdentifierMangled();
    case Decl::Kind::FUNCDECL:
      return std::dynamic_pointer_cast<FuncDecl>(identExpr->getDeclOfIdentifier())->getIdentifierMangled();
    }
  } else {
    reportError("Internal Error - Tried to get identifier from expression");
    return "";
  }
}