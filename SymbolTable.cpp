#include "Symboltable.h"
#include <iostream>

SymbolTable symbolTable;

SymbolTable::SymbolTable()
{
    enterScope(); // Start with global scope
}

void SymbolTable::enterScope()
{
    scopes.emplace_back(); // New empty scope
}

void SymbolTable::exitScope()
{
    if (scopes.empty())
    {
        throw std::runtime_error("No scope to exit.");
    }
    scopes.pop_back();
}

void SymbolTable::declare(const std::string &name, const std::string &type, int line)
{
    if (scopes.empty()) {
        std::cout<<"we are empty (declare)\n";
    }
    auto &currentScope = scopes.back();
    if (currentScope.count(name) > 0)
    {
        std::cout<<"Error at line no "<<line<<": ";
        //throw std::runtime_error("Variable '" + name + "' already declared in this scope.");
        std::cerr<<"Variable '" + name + "' already declared in this scope.\n";
    }
    currentScope.emplace(name, Symbol(name, type, line));
}

const Symbol &SymbolTable::lookup(const std::string &name) const
{
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        auto found = it->find(name);
        if (found != it->end())
        {
            return found->second;
        }
    }
    throw std::runtime_error("Variable '" + name + "' not declared.");
}

bool SymbolTable::isDeclared(const std::string &name) const
{
    if (scopes.empty()) {
        std::cout<<"we are empty (isDeclared) \n"<<name<<"\n";
    }
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        if (it->count(name) > 0)
        {
            return true;
        }
    }
    return false;
}

void SymbolTable::print() const
{
    std::cout << "Symbol Table (from global to inner scopes):\n";
    for (size_t i = 0; i < scopes.size(); ++i)
    {
        std::cout << "  Scope " << i << ":\n";
        for (const auto &pair : scopes[i])
        {
            const auto &sym = pair.second;
            std::cout << "    " << sym.name << " : " << sym.type
                      << " (line " << sym.lineDeclared << ")\n";
        }
    }
}

void SymbolTable::enterLoop() {
    ++loopDepth;
}

void SymbolTable::exitLoop() {
    if (loopDepth > 0)
        --loopDepth;
}

bool SymbolTable::isInsideLoop() const {
    return loopDepth > 0;
}

