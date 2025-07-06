#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>

// Forward declarations (no need to include ast.h here)
class ASTNode;
class BinaryExprNode;

// Represents a declared variable
class Symbol
{
public:
    std::string name;
    std::string type;
    int lineDeclared;

    Symbol(const std::string &name, const std::string &type, int lineDeclared)
        : name(name), type(type), lineDeclared(lineDeclared) {}
};

// Symbol table supporting nested scopes
class SymbolTable
{
public:
    SymbolTable();

    void enterScope(); // Push a new scope
    void exitScope();  // Pop the current scope

    void enterLoop();
    void exitLoop();
    bool isInsideLoop() const;


    void declare(const std::string &name, const std::string &type, int line);
    const Symbol &lookup(const std::string &name) const;
    bool isDeclared(const std::string &name) const;

    void print() const;

    int loopDepth = 0; // For tracking loop depth

private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
};

// Global instance of the symbol table
extern SymbolTable symbolTable;
