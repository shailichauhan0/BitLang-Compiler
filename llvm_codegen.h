// llvm_codegen.h
#pragma once

#include "ast.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

#include <memory>
#include <map>
#include <string>

class LLVMCodeGen {
public:
    LLVMCodeGen();
    void generate(const ProgramNode* root);         // Build LLVM IR from AST
    void dumpIR(const std::string& filename);       // Save IR to file (e.g. output.ll)

private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;
    llvm::Function* mainFunc;
    llvm::BasicBlock* currentBlock;

    std::map<std::string, llvm::AllocaInst*> namedValues;

    // Helpers
    llvm::Value* generateExpr(const ASTNode* expr);
    void generateStmt(const ASTNode* stmt);
};
