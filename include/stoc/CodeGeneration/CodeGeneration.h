//===- stoc/CodeGeneration/CodeGeneration.h - Defintion of CodeGeneration class -----*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file defines the CodeGeneration class.
// CodeGeneration is the fourth, and last, phase of a compiler's frontend and the objective is to
// transform the AST from previous phases into an intermediate representation, usually some form of
// linearization of the AST that is closer to the target language. In this case, code generation
// emits LLVM IR with the help of LLVM API.
//
//===------------------------------------------------------------------------------------------===//

#ifndef STOC_CODEGENERATION_H
#define STOC_CODEGENERATION_H

#include <memory>
#include <unordered_set>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "stoc/AST/Decl.h"
#include "stoc/AST/Expr.h"
#include "stoc/AST/Stmt.h"
#include "stoc/SemanticAnalysis/Type.h"
#include "stoc/SrcFile/SrcFile.h"

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
  std::unordered_map<std::string, llvm::Value *> globalVariables;

  /// Map that relates a function local variable's string identifier with the LLVM value.
  // For every function generated, this map is erased and build.
  std::unordered_map<std::string, llvm::Value *> localVariables;

  std::unordered_set<std::string> builtinFunctions;

  // This basic block is used if a function has multiple returns. In that case, for every return
  // statement, a store to a special return variable is generated and in the end of the function
  // this basic block is appended to return the value of the special variable only once inside the
  // generated LLVM IR function.
  llvm::BasicBlock *exitBB;

  // HELPER METHODS
  /// prints the error \error_msg with information about line and column of the error
  void reportError(std::string error_msg, int line, int column);

  /// prints the error \error_msg
  void reportError(std::string error_msg);

  /// initializes LLVM with information about the target machine for better optimization
  void initialization();

  /// declares the builtin function used to compare strings (strcmp from the C string library)
  void declareStringBuiltinFunctions();

  /// declares the builtin functions print and println (printf from the C stdio library)
  void declareBuiltinFunctions();

  /// returns true if the identifier \functionName is a builtin function in Stoc
  bool isBuiltinFunction(std::string functionName);

  /// generates LLVM IR for calling print/println builtin function in Stoc
  llvm::Value *generateCallBuiltinFunction(std::string functionName,
                                           const std::shared_ptr<CallExpr> &node);

  /// generates LLVM IR for calling print builtin function in Stoc
  llvm::Value *generateCallPrint(const std::shared_ptr<CallExpr> &node);

  /// generates LLVM IR for calling println builtin function in Stoc
  llvm::Value *generateCallPrintln(const std::shared_ptr<CallExpr> &node);

  /// If Expr is an IdentExpr, it return the string of the identifier. If Expr is not an IdentExpr
  /// it ...
  std::string getIdentifier(const std::shared_ptr<Expr> &node);

  /// Returns the LLVM type corresponding to the type of stoc: int (Int64Ty), float (DoubleTy),
  /// bool (Int1Ty), ...
  llvm::Type *getLLVMType(std::shared_ptr<Type> type);

  /// Returns the default value used to initialize a variable in LLVM IR (0 for int, 0.0 for float,
  /// "" for string, false(0) for bool)
  llvm::Constant *getLLVMInit(std::shared_ptr<Type> type);

  /// Generates function to initialize global variables
  void generateFunctionForInitialization(const std::shared_ptr<VarDecl> &node, llvm::Value *GV);

  /// Generates function to initialize global constants
  void generateFunctionForInitialization(const std::shared_ptr<ConstDecl> &node, llvm::Value *GV);

  /// Generates LLVM IR for global variable declarations in Stoc
  void generateGlobalVariableDecl(const std::shared_ptr<VarDecl> &node);

  /// Generates LLVM IR for variable declarations inside functions in Stoc
  void generateLocalVariableDecl(const std::shared_ptr<VarDecl> &node);

  /// Generates LLVM IR for global constants declarations in Stoc
  void generateGlobalConstantDecl(const std::shared_ptr<ConstDecl> &node);

  /// Generates LLVM IR for constants declarations inside functions in Stoc
  void generateLocalConstantDecl(const std::shared_ptr<ConstDecl> &node);

  /// Generates LLVM IR for binary expressions on integer operands
  llvm::Value *generateBinaryExprInt(const std::shared_ptr<BinaryExpr> &node, llvm::Value *lhs,
                                     llvm::Value *rhs);

  /// Generates LLVM IR for binary expressions on float operands
  llvm::Value *generateBinaryExprFloat(const std::shared_ptr<BinaryExpr> &node, llvm::Value *lhs,
                                       llvm::Value *rhs);

  /// Generates LLVM IR for binary expressions on bool operands
  llvm::Value *generateBinaryExprBool(const std::shared_ptr<BinaryExpr> &node, llvm::Value *lhs,
                                      llvm::Value *rhs);

  /// Generates LLVM IR for binary expressions on string operands
  llvm::Value *generateBinaryExprString(const std::shared_ptr<BinaryExpr> &node, llvm::Value *lhs,
                                        llvm::Value *rhs);

  /// Generates LLVM IR for unary expressions on integer operand
  llvm::Value *generateUnaryExprInt(const std::shared_ptr<UnaryExpr> &node, llvm::Value *rhs);

  /// Generates LLVM IR for unary expressions on float operand
  llvm::Value *generateUnaryExprFloat(const std::shared_ptr<UnaryExpr> &node, llvm::Value *rhs);

  /// Generates LLVM IR for bool expressions on bool operands
  llvm::Value *generateUnaryExprBool(const std::shared_ptr<UnaryExpr> &node, llvm::Value *rhs);

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

  /// prints the LLVM IR generated
  void printLLVM();

  /// must call after generating the LLVM IR. Transfoms LLVM IR into an executable by invoking other
  /// programs like the llc (LLVM static compiler) and gcc (GNU C Compiler that invokes the linker)
  void getExecutable();
};

#endif // STOC_CODEGENERATION_H
