#include "ast_printer.h"
#include "ast.h"
#include <iostream>
#include <memory>

extern std::unique_ptr<ProgramNode> astRoot;

// Print any given AST
void printAST(const ProgramNode &program)
{
    program.print();
}
// Print the global AST
void printGlobalAST()
{
    if (astRoot)
    {
        std::cout << "Printing the AST:\n";
        printAST(*astRoot);
    }
    else
    {
        std::cout << "AST is empty or not generated yet.\n";
    }
}
