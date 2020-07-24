#ifndef STOC_CODEGENERATION_H
#define STOC_CODEGENERATION_H

#include <memory>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "stoc/AST/Decl.h"
#include "stoc/AST/Expr.h"
#include "stoc/AST/Stmt.h"
#include "stoc/SrcFile/SrcFile.h"

// This class will visit every node recursively (but without implementing visitor pattern)
/// Phase of the compiler that translates the AST (after Semantic Analysis) into Intermediate
/// Representation. Because the LLVM tools are used, the output of this phase is LLVM IR.
class CodeGeneration {
  // This class will visit every AST node and generate the code of that node. It will traverse
  // the AST nodes recursively but without implementing the visitor pattern used in other parts of
  // the stoc compiler, because for this phase, some of the methods have to return some value when
  // visiting a node.
private:
  std::shared_ptr<SrcFile> file; /// stoc source file, list of tokens and AST

  // State of the Code Generator
  // LLVM specifics:
  llvm::LLVMContext context;
  std::shared_ptr<llvm::Module> module;
  std::shared_ptr<llvm::IRBuilder<>> builder;

  /// Map that relates a global variable's string identifier with the LLVM value
  std::unordered_map<std::string, llvm::GlobalVariable *> globalVariables;

  /// Map that relates a function local variable's string identifier with the LLVM value.
  // For every function generated, this map is erased and build.
  std::unordered_map<std::string, llvm::Value *> localVariables;

  // This basic block is used if a function has multiple returns. In that case, for every return
  // statement, a store to a special return variable is generated and in the end of the function
  // this basic block is appended to return the value of the special variable only once inside the
  // generated LLVM IR function.
  llvm::BasicBlock *exitBB;

  // HELPER METHODS
  /// If Expr is an IdentExpr, it return the string of the identifier. If Expr is not an IdentExpr
  /// it ...
  // TODO: see what happens if node is not IdentExpr
  std::string getIdentifier(const std::shared_ptr<Expr> &node);

  /// Returns the LLVM type corresponding to the type of stoc: int (Int64Ty), float (DoubleTy),
  /// bool (Int1Ty), ...
  llvm::Type *getLLVMType(std::string type);

  /// Generates function to initialize global variables
  void generateFunctionForInitialization(const std::shared_ptr<VarDecl> &node,
                                         llvm::Value *GV);

  /// Generates function to initialize global constants
  void generateFunctionForInitialization(const std::shared_ptr<ConstDecl> &node,
                                         llvm::Value *GV);

  // MAIN METHODS
  llvm::Value *generate(const std::shared_ptr<Expr> &node);
  void generate(const std::shared_ptr<Decl> &node);
  void generate(const std::shared_ptr<Stmt> &node);

  void generate(const std::shared_ptr<VarDecl> &node);
  void generate(const std::shared_ptr<ConstDecl> &node);
  void generate(const std::shared_ptr<ParamDecl> &node);
  void generate(const std::shared_ptr<FuncDecl> &node);

  void generate(const std::shared_ptr<DeclarationStmt> &node);
  void generate(const std::shared_ptr<ExpressionStmt> &node);
  void generate(const std::shared_ptr<BlockStmt> &node);
  void generate(const std::shared_ptr<IfStmt> &node);
  void generate(const std::shared_ptr<ForStmt> &node);
  void generate(const std::shared_ptr<WhileStmt> &node);
  void generate(const std::shared_ptr<AssignmentStmt> &node);
  void generate(const std::shared_ptr<ReturnStmt> &node);

  llvm::Value *generate(const std::shared_ptr<BinaryExpr> &node);
  llvm::Value *generate(const std::shared_ptr<UnaryExpr> &node);
  llvm::Value *generate(const std::shared_ptr<LiteralExpr> &node);
  llvm::Value *generate(const std::shared_ptr<IdentExpr> &node);
  llvm::Value *generate(const std::shared_ptr<CallExpr> &node);

public:
  explicit CodeGeneration(std::shared_ptr<SrcFile> &file);

  /// main method: generates LLVM IR code
  void generate();
};

#endif // STOC_CODEGENERATION_H
