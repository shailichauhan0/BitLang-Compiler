// ast.h
#pragma once
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include "SymbolTable.h"

// Base class for all AST nodes
class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual void print() const = 0;
    virtual std::string analyze(SymbolTable &symbols) const = 0; // For expressions and type-checking
    int lineNumber;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

// ===== Expression Nodes =====

class LiteralNode : public ASTNode
{
public:
    enum class Type
    {
        Int,
        Float,
        String,
        Char,
        Bool
    };
    Type type;
    std::string value;

    LiteralNode(Type t, const std::string &val) : type(t), value(val) {}

    void print() const override
    {
        std::cout << "Literal(" << value << ")";
    }
    std::string analyze(SymbolTable &symbols) const override;
};

class IdentifierNode : public ASTNode
{
public:
    std::string name;

    IdentifierNode(const std::string &id) : name(id) {}

    std::string analyze(SymbolTable &symbols) const override;

    void print() const override
    {
        std::cout << "Identifier(" << name << ")";
    }
};

class BinaryExprNode : public ASTNode
{
public:
    enum class Op
    {
        Add,
        Sub,
        Mul,
        Div,
        Eq,
        Neq,
        Lt,
        Gt,
        Leq,
        Geq,
        And,
        Or
    };
    ASTNodePtr left;
    ASTNodePtr right;
    Op op;

    BinaryExprNode(ASTNodePtr lhs, Op oper, ASTNodePtr rhs)
        : left(std::move(lhs)), op(oper), right(std::move(rhs)) {}

    std::string analyze(SymbolTable &symbols) const override;

    void print() const override
    {
        std::cout << "(";
        left->print();
        switch (op)
        {
        case Op::Add:
            std::cout << " + ";
            break;
        case Op::Sub:
            std::cout << " - ";
            break;
        case Op::Mul:
            std::cout << " * ";
            break;
        case Op::Div:
            std::cout << " / ";
            break;
        case Op::Eq:
            std::cout << " == ";
            break;
        case Op::Neq:
            std::cout << " != ";
            break;
        case Op::Lt:
            std::cout << " < ";
            break;
        case Op::Gt:
            std::cout << " > ";
            break;
        case Op::Leq:
            std::cout << " <= ";
            break;
        case Op::Geq:
            std::cout << " >= ";
            break;
        case Op::And:
            std::cout << " and ";
            break;
        case Op::Or:
            std::cout << " or ";
            break;
        }
        right->print();
        std::cout << ")";
    }
};

class UnaryExprNode : public ASTNode
{
public:
    enum class Op
    {
        Not,
        Minus
    };

    Op op;
    ASTNodePtr operand;

    UnaryExprNode(Op o, ASTNodePtr expr)
        : op(o), operand(std::move(expr)) {}

    void print() const override
    {
        std::cout << "Unary(";
        switch (op)
        {
        case Op::Not:
            std::cout << "not ";
            break;
        case Op::Minus:
            std::cout << "-";
            break;
        }
        operand->print();
        std::cout << ")";
    }
    std::string analyze(SymbolTable &symbols) const override;
};

// ===== Statement Nodes =====

class DeclarationNode : public ASTNode
{
public:
    std::string typeName;
    std::string identifier;
    ASTNodePtr expr;

    DeclarationNode(const std::string &type, const std::string &id, ASTNodePtr e)
        : typeName(type), identifier(id), expr(std::move(e)) {}

    std::string analyze(SymbolTable &symbols) const override;

    void print() const override
    {
        std::cout << "Declare(" << typeName << " " << identifier << " = ";
        expr->print();
        std::cout << ")";
    }
};

class PrintStmtNode : public ASTNode
{
public:
    ASTNodePtr expr;

    PrintStmtNode(ASTNodePtr e) : expr(std::move(e)) {}

    std::string analyze(SymbolTable &symbols) const override;

    void print() const override
    {
        std::cout << "Print(";
        expr->print();
        std::cout << ")";
    }
};

class ReturnStmtNode : public ASTNode
{
public:
    ASTNodePtr expr;

    ReturnStmtNode(ASTNodePtr e) : expr(std::move(e)) {}

    std::string analyze(SymbolTable &symbols) const override;

    void print() const override
    {
        std::cout << "Return(";
        expr->print();
        std::cout << ")";
    }
};

class IfStmtNode : public ASTNode
{
public:
    ASTNodePtr condition;
    ASTNodePtr thenBlock;
    ASTNodePtr elseBlock; // optional

    IfStmtNode(ASTNodePtr cond, ASTNodePtr thenBlk, ASTNodePtr elseBlk = nullptr)
        : condition(std::move(cond)), thenBlock(std::move(thenBlk)), elseBlock(std::move(elseBlk)) {}

    std::string analyze(SymbolTable &symbols) const override;

    void print() const override
    {
        std::cout << "If(";
        condition->print();
        std::cout << ") Then ";
        thenBlock->print();
        if (elseBlock)
        {
            std::cout << " Else ";
            elseBlock->print();
        }
    }
};

class RepeatStmtNode : public ASTNode
{
public:
    ASTNodePtr condition;
    ASTNodePtr body;

    RepeatStmtNode(ASTNodePtr cond, ASTNodePtr blk)
        : condition(std::move(cond)), body(std::move(blk)) {}

    std::string analyze(SymbolTable &symbols) const override;

    void print() const override
    {
        std::cout << "Repeat(";
        condition->print();
        std::cout << ") ";
        body->print();
    }
};

class AssignmentNode : public ASTNode
{
public:
    std::string name;
    std::unique_ptr<ASTNode> value;

    AssignmentNode(std::string name, std::unique_ptr<ASTNode> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::string analyze(SymbolTable &symbols) const override;

    void print() const override
    {
        std::cout << "Assignment(" << name << " = ";
        value->print();
        std::cout << ")";
    }
};

class BlockNode : public ASTNode
{
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    BlockNode(std::unique_ptr<std::vector<std::unique_ptr<ASTNode>>> stmts)
        : statements(std::move(*stmts)) {} // move vector contents

    std::string analyze(SymbolTable &symbols) const override;

    void print() const override
    {
        std::cout << "{ ";
        for (const auto &stmt : statements)
        {
            stmt->print();
            std::cout << "; ";
        }
        std::cout << "}";
    }
};

/*class BlockNode : public ASTNode
{
public:
    std::vector<ASTNodePtr> statements;

    void addStatement(ASTNodePtr stmt)
    {
        statements.push_back(std::move(stmt));
    }

    void print() const override
    {
        std::cout << "{ ";
        for (const auto &stmt : statements)
        {
            stmt->print();
            std::cout << "; ";
        }
        std::cout << "}";
    }
};*/

// ===== Program Node (Root) =====

class ProgramNode : public ASTNode
{
public:
    std::vector<ASTNodePtr> statements;

    void addStatement(ASTNodePtr stmt)
    {
        statements.push_back(std::move(stmt));
    }

    void print() const override
    {
        std::cout << "Program:\n";
        for (const auto &stmt : statements)
        {
            stmt->print();
            std::cout << "\n";
        }
    }
    std::string analyze(SymbolTable &symbols) const override;
};

class BreakNode : public ASTNode {
    public:
        int line;
        BreakNode(int line) : line(line) {}
        std::string analyze(SymbolTable & analyzer) const override;
        void print() const override
        {
            std::cout << " {stop}  ";
        }
    };
    
class ContinueNode : public ASTNode {
    public:
        int line;
        ContinueNode(int line) : line(line) {}
        std::string analyze(SymbolTable & analyzer) const override;
        void print() const override
        {
            std::cout << " {skip}  ";
        }
    };
    

class BuiltinCallNode : public ASTNode
{
public:
    std::string funcName;
    std::vector<ASTNodePtr> args;

    BuiltinCallNode(const std::string &name, std::vector<ASTNodePtr> arguments)
        : funcName(name), args(std::move(arguments)) {}

    void print() const override
    {
        std::cout << "BuiltinCall(" << funcName << "(";
        for (size_t i = 0; i < args.size(); ++i)
        {
            args[i]->print();
            if (i + 1 < args.size())
                std::cout << ", ";
        }
        std::cout << "))";
    }
    std::string analyze(SymbolTable &symbols) const override;
};
