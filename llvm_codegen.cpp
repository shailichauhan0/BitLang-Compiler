// llvm_codegen.cpp
#include "llvm_codegen.h"
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>

LLVMCodeGen::LLVMCodeGen() : builder(context) {
    module = std::make_unique<llvm::Module>("MyModule", context);
}

void LLVMCodeGen::generate(const ProgramNode* root) {
    llvm::FunctionType* mainType = llvm::FunctionType::get(builder.getInt32Ty(), false);
    mainFunc = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", module.get());
    currentBlock = llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(currentBlock);

    for (const auto& stmt : root->statements) {
        generateStmt(stmt.get());
    }

    builder.CreateRet(builder.getInt32(0));
    llvm::verifyFunction(*mainFunc);
}

void LLVMCodeGen::dumpIR(const std::string& filename) {
    std::error_code EC;
    llvm::raw_fd_ostream out(filename, EC);
    module->print(out, nullptr);
}

llvm::Value* LLVMCodeGen::generateExpr(const ASTNode* expr) {
    if (auto lit = dynamic_cast<const LiteralNode*>(expr)) {
        if (lit->type == LiteralNode::Type::Int)
            return llvm::ConstantInt::get(builder.getInt32Ty(), std::stoi(lit->value));
        if (lit->type == LiteralNode::Type::Float)
            return llvm::ConstantFP::get(builder.getFloatTy(), std::stof(lit->value));
        if (lit->type == LiteralNode::Type::Bool)
            return llvm::ConstantInt::get(builder.getInt1Ty(), lit->value == "true" ? 1 : 0);
        if (lit->type == LiteralNode::Type::String)
            return builder.CreateGlobalStringPtr(lit->value);
    } else if (auto ident = dynamic_cast<const IdentifierNode*>(expr)) {
        llvm::AllocaInst* ptr = namedValues[ident->name];
        return builder.CreateLoad(ptr->getAllocatedType(), ptr);
    } else if (auto builtin = dynamic_cast<const BuiltinCallNode*>(expr)) {
        if (builtin->funcName == "input") {
            llvm::FunctionType* scanfType = llvm::FunctionType::get(builder.getInt32Ty(), true);
            llvm::FunctionCallee scanfFunc = module->getOrInsertFunction("scanf", scanfType);

            std::string format = "%d";  // default int
            llvm::Type* allocType = builder.getInt32Ty();

            if (!builtin->args.empty()) {
                auto typeHint = dynamic_cast<const LiteralNode*>(builtin->args[0].get());
                if (typeHint && typeHint->type == LiteralNode::Type::String) {
                    std::string hint = typeHint->value;
                    if (hint == "int") {
                        format = "%d";
                        allocType = builder.getInt32Ty();
                    } else if (hint == "float") {
                        format = "%f";
                        allocType = builder.getFloatTy();
                    } else if (hint == "bool") {
                        format = "%d";
                        allocType = builder.getInt1Ty();
                    } else if (hint == "string") {
                        format = "%s";
                        allocType = llvm::ArrayType::get(builder.getInt8Ty(), 256);
                    }
                }
            }

            llvm::AllocaInst* temp = builder.CreateAlloca(allocType);

            llvm::Value* formatStr = builder.CreateGlobalStringPtr(format);
            if (format == "%s") {
                builder.CreateCall(scanfFunc, {formatStr, temp});
                return temp; // pointer to string
            } else {
                builder.CreateCall(scanfFunc, {formatStr, temp});
                return builder.CreateLoad(allocType, temp);
            }
        }
    } else if (auto bin = dynamic_cast<const BinaryExprNode*>(expr)) {
        auto L = generateExpr(bin->left.get());
        auto R = generateExpr(bin->right.get());
        llvm::Type* ty = L->getType();  // 🔁 MODIFIED

        switch (bin->op) {
            case BinaryExprNode::Op::Add:
                return ty->isFloatingPointTy() ? builder.CreateFAdd(L, R) : builder.CreateAdd(L, R); // 🔁
            case BinaryExprNode::Op::Sub:
                return ty->isFloatingPointTy() ? builder.CreateFSub(L, R) : builder.CreateSub(L, R); // 🔁
            case BinaryExprNode::Op::Mul:
                return ty->isFloatingPointTy() ? builder.CreateFMul(L, R) : builder.CreateMul(L, R); // 🔁
            case BinaryExprNode::Op::Div:
                return ty->isFloatingPointTy() ? builder.CreateFDiv(L, R) : builder.CreateSDiv(L, R); // 🔁

            case BinaryExprNode::Op::Eq:
                return ty->isFloatingPointTy() ? builder.CreateFCmpUEQ(L, R) : builder.CreateICmpEQ(L, R); // 🔁
            case BinaryExprNode::Op::Neq:
                return ty->isFloatingPointTy() ? builder.CreateFCmpUNE(L, R) : builder.CreateICmpNE(L, R); // 🔁
            case BinaryExprNode::Op::Lt:
                return ty->isFloatingPointTy() ? builder.CreateFCmpULT(L, R) : builder.CreateICmpSLT(L, R); // 🔁
            case BinaryExprNode::Op::Gt:
                return ty->isFloatingPointTy() ? builder.CreateFCmpUGT(L, R) : builder.CreateICmpSGT(L, R); // 🔁
            case BinaryExprNode::Op::Leq:
                return ty->isFloatingPointTy() ? builder.CreateFCmpULE(L, R) : builder.CreateICmpSLE(L, R); // 🔁
            case BinaryExprNode::Op::Geq:
                return ty->isFloatingPointTy() ? builder.CreateFCmpUGE(L, R) : builder.CreateICmpSGE(L, R); // 🔁

            case BinaryExprNode::Op::And:
            case BinaryExprNode::Op::Or:
                return bin->op == BinaryExprNode::Op::And ? builder.CreateAnd(L, R) : builder.CreateOr(L, R);
            default: return nullptr;
        }
    } else if (auto un = dynamic_cast<const UnaryExprNode*>(expr)) {
        llvm::Value* val = generateExpr(un->operand.get());
        if (un->op == UnaryExprNode::Op::Minus)
            return val->getType()->isFloatingPointTy() ? builder.CreateFNeg(val) : builder.CreateNeg(val); // 🔁
        if (un->op == UnaryExprNode::Op::Not)
            return builder.CreateNot(val);
    }
    return nullptr;
}

void LLVMCodeGen::generateStmt(const ASTNode* stmt) {
    if (auto decl = dynamic_cast<const DeclarationNode*>(stmt)) {
        llvm::AllocaInst* alloc;
        if (decl->typeName == "int") {
            alloc = builder.CreateAlloca(builder.getInt32Ty(), nullptr, decl->identifier);
        } else if (decl->typeName == "float") {
            alloc = builder.CreateAlloca(builder.getFloatTy(), nullptr, decl->identifier);
        } else if (decl->typeName == "bool") {
            alloc = builder.CreateAlloca(builder.getInt1Ty(), nullptr, decl->identifier);
        } else {
            alloc = builder.CreateAlloca(builder.getInt32Ty(), nullptr, decl->identifier);
        }
        llvm::Value* initVal = generateExpr(decl->expr.get());
        builder.CreateStore(initVal, alloc);
        namedValues[decl->identifier] = alloc;

    } else if (auto print = dynamic_cast<const PrintStmtNode*>(stmt)) {
        llvm::FunctionType* printfType = llvm::FunctionType::get(builder.getInt32Ty(), true);
        llvm::FunctionCallee printfFunc = module->getOrInsertFunction("printf", printfType);

        llvm::Value* val = generateExpr(print->expr.get());
        llvm::Type* ty = val->getType();

        llvm::Value* formatStr;

        // Handle printing of string literals separately
        if (auto lit = dynamic_cast<const LiteralNode*>(print->expr.get())) {
            if (lit->type == LiteralNode::Type::String) {
                formatStr = builder.CreateGlobalStringPtr("%s\n");
                builder.CreateCall(printfFunc, {formatStr, val});
        return;
    }
}


        if (ty->isFloatTy()) {
            // Promote float to double for printf varargs
            val = builder.CreateFPExt(val, builder.getDoubleTy());
            formatStr = builder.CreateGlobalStringPtr("%f\n");

        } else if (ty->isDoubleTy()) {
            formatStr = builder.CreateGlobalStringPtr("%f\n");

        } else if (ty->isIntegerTy(1) || ty->isIntegerTy(32)) {
            formatStr = builder.CreateGlobalStringPtr("%d\n");

        } else {
            formatStr = builder.CreateGlobalStringPtr("[unsupported type]\n");
        }

        builder.CreateCall(printfFunc, {formatStr, val});

    } else if (auto assign = dynamic_cast<const AssignmentNode*>(stmt)) {
        llvm::Value* val = generateExpr(assign->value.get());
        builder.CreateStore(val, namedValues[assign->name]);

    } else if (auto ifStmt = dynamic_cast<const IfStmtNode*>(stmt)) {
        llvm::Value* condVal = generateExpr(ifStmt->condition.get());
        if (condVal->getType()->isIntegerTy() && condVal->getType()->getIntegerBitWidth() != 1) {
            condVal = builder.CreateICmpNE(condVal, llvm::ConstantInt::get(condVal->getType(), 0));
        }
        llvm::Function* func = builder.GetInsertBlock()->getParent();

        llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(context, "then", func);
        llvm::BasicBlock* elseBB = ifStmt->elseBlock ? llvm::BasicBlock::Create(context, "else") : nullptr;
        llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "ifcont");

        if (elseBB)
            builder.CreateCondBr(condVal, thenBB, elseBB);
        else
            builder.CreateCondBr(condVal, thenBB, mergeBB);

        builder.SetInsertPoint(thenBB);
        generateStmt(ifStmt->thenBlock.get());
        builder.CreateBr(mergeBB);

        if (elseBB) {
            elseBB->insertInto(func);
            builder.SetInsertPoint(elseBB);
            generateStmt(ifStmt->elseBlock.get());
            builder.CreateBr(mergeBB);
        }

        mergeBB->insertInto(func);
        builder.SetInsertPoint(mergeBB);

    } else if (auto repeat = dynamic_cast<const RepeatStmtNode*>(stmt)) {
        llvm::Function* func = builder.GetInsertBlock()->getParent();

        llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(context, "loop", func);
        llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(context, "afterloop");

        builder.CreateBr(loopBB);
        builder.SetInsertPoint(loopBB);

        generateStmt(repeat->body.get());

        llvm::Value* condVal = generateExpr(repeat->condition.get());
        if (condVal->getType()->isIntegerTy() && condVal->getType()->getIntegerBitWidth() != 1) {
            condVal = builder.CreateICmpNE(condVal, llvm::ConstantInt::get(condVal->getType(), 0));
        }
        builder.CreateCondBr(condVal, loopBB, afterBB);

        afterBB->insertInto(func);
        builder.SetInsertPoint(afterBB);

    } else if (auto block = dynamic_cast<const BlockNode*>(stmt)) {
        for (const auto& s : block->statements) {
            generateStmt(s.get());
        }
    }
}

