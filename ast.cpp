#include "ast.h"
#include "ast_interface.h"
#include "SymbolTable.h"

#include <iostream>
#include <memory>

#include <iostream>

// Global AST root
std::unique_ptr<ProgramNode> astRoot = nullptr;

// -------------------- Literal Builders --------------------
std::unique_ptr<LiteralNode> makeIntLiteral(int value, int line)
{
    auto node = std::make_unique<LiteralNode>(LiteralNode::Type::Int, std::to_string(value));
    node->lineNumber = line;

    return node;
}

std::unique_ptr<LiteralNode> makeFloatLiteral(float value, int line)
{
    auto node = std::make_unique<LiteralNode>(LiteralNode::Type::Float, std::to_string(value));
    node->lineNumber = line;
    return node;
}

std::unique_ptr<LiteralNode> makeStringLiteral(const std::string &value, int line)
{
    auto node = std::make_unique<LiteralNode>(LiteralNode::Type::String, value);
    node->lineNumber = line;
    return node;
}

std::unique_ptr<LiteralNode> makeCharLiteral(char value, int line)
{
    auto node = std::make_unique<LiteralNode>(LiteralNode::Type::Char, std::string(1, value));
    node->lineNumber = line;
    return node;
}

std::unique_ptr<LiteralNode> makeBoolLiteral(bool value, int line)
{
    auto node = std::make_unique<LiteralNode>(LiteralNode::Type::Bool, value ? "true" : "false");
    node->lineNumber = line;
    return node;
}

// -------------------- Identifier --------------------
std::unique_ptr<IdentifierNode> makeIdentifier(const std::string &name, int line)
{
    auto node = std::make_unique<IdentifierNode>(name);
    node->lineNumber = line;
    return node;
}

// -------------------- Expressions --------------------
std::unique_ptr<BinaryExprNode> makeBinaryExpr(
    std::unique_ptr<ASTNode> left,
    BinaryExprNode::Op op,
    std::unique_ptr<ASTNode> right,
    int line)
{
    auto node = std::make_unique<BinaryExprNode>(std::move(left), op, std::move(right));
    node->lineNumber = line;
    return node;
}

std::unique_ptr<UnaryExprNode> makeUnaryExpr(UnaryExprNode::Op op, std::unique_ptr<ASTNode> operand, int line)
{
    auto node = std::make_unique<UnaryExprNode>(op, std::move(operand));
    node->lineNumber = line;
    return node;
}

// -------------------- Statements --------------------
std::unique_ptr<DeclarationNode> makeDeclaration(
    const std::string &type,
    const std::string &name,
    std::unique_ptr<ASTNode> expr,
    int line)
{
    auto node = std::make_unique<DeclarationNode>(type, name, std::move(expr));
    node->lineNumber = line;
    //std::cout << "d line no is" << line << std::endl;
    return node;
}

std::unique_ptr<PrintStmtNode> makePrintStmt(std::unique_ptr<ASTNode> expr, int line)
{
    auto node = std::make_unique<PrintStmtNode>(std::move(expr));
    node->lineNumber = line;
    return node;
}

std::unique_ptr<ReturnStmtNode> makeReturnStmt(std::unique_ptr<ASTNode> expr, int line)
{
    auto node = std::make_unique<ReturnStmtNode>(std::move(expr));
    node->lineNumber = line;
    return node;
}

std::unique_ptr<IfStmtNode> makeIfStmt(
    std::unique_ptr<ASTNode> condition,
    std::unique_ptr<ASTNode> thenBlock,
    std::unique_ptr<ASTNode> elseBlock,
    int line)
{
    auto node = std::make_unique<IfStmtNode>(std::move(condition), std::move(thenBlock), std::move(elseBlock));
    node->lineNumber = line;
    return node;
}

std::unique_ptr<RepeatStmtNode> makeRepeatStmt(
    std::unique_ptr<ASTNode> condition,
    std::unique_ptr<ASTNode> body,
    int line)
{
    auto node = std::make_unique<RepeatStmtNode>(std::move(condition), std::move(body));
    node->lineNumber = line;
    return node;
}

// -------------------- Assignment --------------------
std::unique_ptr<ASTNode> makeAssignment(const std::string &name, std::unique_ptr<ASTNode> expr, int line)
{
    auto node = std::make_unique<AssignmentNode>(name, std::move(expr));
    node->lineNumber = line;
    return node;
}

// -------------------- Block --------------------
std::unique_ptr<BlockNode> makeBlock(std::unique_ptr<std::vector<std::unique_ptr<ASTNode>>> statements, int line)
{
    auto node = std::make_unique<BlockNode>(std::move(statements));
    node->lineNumber = line;
    return node;
}

void addToBlock(BlockNode *block, std::unique_ptr<ASTNode> stmt)
{
    block->statements.push_back(std::move(stmt));
}

// -------------------- Program --------------------
std::unique_ptr<ProgramNode> makeProgram()
{
    auto node = std::make_unique<ProgramNode>();
    // node->lineNumber = line;
    return node;
}

void addToProgram(std::unique_ptr<ASTNode> stmt)
{
    if (!stmt)
        return;
    if (!astRoot)
        astRoot = std::make_unique<ProgramNode>();
    astRoot->statements.push_back(std::move(stmt));
}

// -------------------- Break/Continue --------------------
std::unique_ptr<BreakNode> makeBreak(int line)
{
    auto node = std::make_unique<BreakNode>(line);
    node->lineNumber = line;
    return node;
}

std::unique_ptr<ContinueNode> makeContinue(int line)
{
    auto node = std::make_unique<ContinueNode>(line);
    node->lineNumber = line;
    return node;
}

// -------------------- Builtin Call --------------------
std::unique_ptr<BuiltinCallNode> makeBuiltinCall(const std::string &name, std::vector<ASTNodePtr> args, int line)
{
    auto node = std::make_unique<BuiltinCallNode>(name, std::move(args));
    node->lineNumber = line;
    return node;
}




//---Symanitc Analysis---

std::string BreakNode::analyze(SymbolTable &symbols) const
{
    if (symbols.loopDepth == 0) {
        std::cerr << "Semantic Error at line " << line << ": 'stop' used outside of loop.\n";
    }
    return "void";
}

std::string ContinueNode::analyze(SymbolTable &symbols) const
{
    if (symbols.loopDepth == 0) {
        std::cerr << "Semantic Error at line " << line << ": 'skip' used outside of loop.\n";
    }
    return "void";
}

std::string LiteralNode::analyze(SymbolTable &symbols) const
{
    switch (type)
    {
    case Type::Int:
        return "int";
    case Type::Float:
        return "float";
    case Type::String:
        return "string";
    case Type::Char:
        return "char";
    case Type::Bool:
        return "bool";
    }
    return "unknown";
}

std::string IdentifierNode::analyze(SymbolTable &symbols) const
{
    try{
        const Symbol &result = symbols.lookup(name);
        return result.type;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return "error";
    }
}

std::string DeclarationNode::analyze(SymbolTable &symbols) const
{
    std::string exprType = expr->analyze(symbols);
    if (exprType != typeName)
    {
        std::cerr << "Type mismatch in declaration of '" << identifier
                  << "': expected " << typeName << ", got " << exprType << "\n";
    }
    symbols.declare(identifier, typeName, lineNumber);
    return "void";
}

std::string AssignmentNode::analyze(SymbolTable &symbols) const
{
    try
    {
        const Symbol &declaredSymbol = symbols.lookup(name);
        std::string valueType = value->analyze(symbols);

        if (declaredSymbol.type != valueType)
        {
            std::cerr << "Type mismatch in assignment to '" << name
                      << "': expected " << declaredSymbol.type << ", got " << valueType << "\n";
        }

        return "void";
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return "error";
    }
}

std::string BinaryExprNode::analyze(SymbolTable &symbols) const
{
    std::string leftType = left->analyze(symbols);
    std::string rightType = right->analyze(symbols);

    if (leftType != rightType)
    {
        std::cerr << "Type mismatch in binary expression: " << leftType << " vs " << rightType << "\n";
        return "error";
    }

    switch (op)
    {
    case Op::Add:
    case Op::Sub:
    case Op::Mul:
    case Op::Div:
        return (leftType == "int" || leftType == "float") ? leftType : "error";

    case Op::Eq:
    case Op::Neq:
    case Op::Lt:
    case Op::Gt:
    case Op::Leq:
    case Op::Geq:
        return "bool";

    case Op::And:
    case Op::Or:
        if (leftType != "bool")
            std::cerr << "Logical operators require boolean types\n";
        return "bool";
    }
    return "error";
}

std::string UnaryExprNode::analyze(SymbolTable &symbols) const
{
    std::string operandType = operand->analyze(symbols);
    if (op == Op::Not && operandType != "bool")
    {
        std::cerr << "Error: 'not' operator requires a boolean operand\n";
        return "error";
    }
    if (op == Op::Minus && operandType != "int" && operandType != "float")
    {
        std::cerr << "Error: '-' operator requires an integer or float operand\n";
        return "error";
    }
    return operandType;
}

std::string BlockNode::analyze(SymbolTable &symbols) const
{
    symbols.enterScope();
    for (const auto &stmt : statements)
    {
        stmt->analyze(symbols);
    }
    symbols.exitScope();
    return "void";
}

std::string ProgramNode::analyze(SymbolTable &symbols) const
{
    //symbols.enterScope();
    for (const auto &stmt : statements)
    {
        stmt->analyze(symbols);
    }
    //symbols.exitScope();
    return "void";
}

std::string IfStmtNode::analyze(SymbolTable &symbols) const
{
    std::string condType = condition->analyze(symbols);
    if (condType != "bool")
    {
        std::cerr << "Line " << lineNumber << ": Condition in if statement must be of type 'bool', got '" << condType << "'\n";
    }

    //symbols.enterScope();
    thenBlock->analyze(symbols);
    //symbols.exitScope();

    if (elseBlock)
    {
        //symbols.enterScope();
        elseBlock->analyze(symbols);
        //symbols.exitScope();
    }

    return "void";
}

std::string RepeatStmtNode::analyze(SymbolTable &symbols) const
{
    std::string condType = condition->analyze(symbols);
    if (condType != "bool")
    {
        std::cerr << "Line " << lineNumber << ": Condition in repeat statement must be of type 'bool', got '" << condType << "'\n";
    }
    
    symbolTable.enterLoop();
    //symbols.enterScope();
    body->analyze(symbols);
    //symbols.exitScope();
    symbolTable.exitLoop();

    return "void";
}

std::string ReturnStmtNode::analyze(SymbolTable &symbols) const
{
    std::string exprType = expr->analyze(symbols);
    std::cout << "Line " << lineNumber << ": return " << exprType << "\n";
    // You can extend this later with function return type checking.
    return exprType;
}

std::string PrintStmtNode::analyze(SymbolTable &symbols) const
{
    expr->analyze(symbols); // Analyze the expression being printed
    return "void";
}

std::string BuiltinCallNode::analyze(SymbolTable &symbols) const
{

    return "unknown"; // You can update this later with proper return types
}
