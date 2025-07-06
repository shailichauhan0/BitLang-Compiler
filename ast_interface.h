#pragma once
#include "ast.h"
#include <memory>

// The root of the AST will be stored here
extern std::unique_ptr<ProgramNode> astRoot;

// Functions to build AST nodes — called from parser actions
std::unique_ptr<LiteralNode> makeIntLiteral(int value, int line);
std::unique_ptr<LiteralNode> makeFloatLiteral(float value, int line);
std::unique_ptr<LiteralNode> makeStringLiteral(const std::string &value, int line);
std::unique_ptr<LiteralNode> makeCharLiteral(char value, int line);
std::unique_ptr<LiteralNode> makeBoolLiteral(bool value, int line);

std::unique_ptr<IdentifierNode> makeIdentifier(const std::string &name, int line);

std::unique_ptr<BinaryExprNode> makeBinaryExpr(
    std::unique_ptr<ASTNode> left,
    BinaryExprNode::Op op,
    std::unique_ptr<ASTNode> right,
    int line);

std::unique_ptr<UnaryExprNode> makeUnaryExpr(
    UnaryExprNode::Op op,
    std::unique_ptr<ASTNode> operand,
    int line);

std::unique_ptr<DeclarationNode> makeDeclaration(
    const std::string &type,
    const std::string &name,
    std::unique_ptr<ASTNode> expr,
    int line);

std::unique_ptr<PrintStmtNode> makePrintStmt(std::unique_ptr<ASTNode> expr, int line);
std::unique_ptr<ReturnStmtNode> makeReturnStmt(std::unique_ptr<ASTNode> expr, int line);

std::unique_ptr<IfStmtNode> makeIfStmt(
    std::unique_ptr<ASTNode> condition,
    std::unique_ptr<ASTNode> thenBlock,
    std::unique_ptr<ASTNode> elseBlock,
    int line);

std::unique_ptr<RepeatStmtNode> makeRepeatStmt(
    std::unique_ptr<ASTNode> condition,
    std::unique_ptr<ASTNode> body,
    int line);

std::unique_ptr<ASTNode> makeAssignment(
    const std::string &name,
    std::unique_ptr<ASTNode> expr,
    int line);

std::unique_ptr<BlockNode> makeBlock(
    std::unique_ptr<std::vector<std::unique_ptr<ASTNode>>> stmts,
    int line);

void addToBlock(BlockNode *block, std::unique_ptr<ASTNode> stmt);

std::unique_ptr<ProgramNode> makeProgram();
void addToProgram(std::unique_ptr<ASTNode> stmt);

std::unique_ptr<BreakNode> makeBreak(int line);
std::unique_ptr<ContinueNode> makeContinue(int line);

std::unique_ptr<BuiltinCallNode> makeBuiltinCall(
    const std::string &name,
    std::vector<ASTNodePtr> args,
    int line);
