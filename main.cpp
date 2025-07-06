#include "ast_interface.h"
#include "ast.h"
#include "SymbolTable.h"
#include "llvm_codegen.h"  // NEW

#include <iostream>
#include <fstream>

extern "C" int yyparse();
extern FILE *yyin;
extern std::unique_ptr<ProgramNode> astRoot;
extern SymbolTable symbolTable;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: ./compiler <source-file>\n";
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        std::cerr << "Could not open file " << argv[1] << "\n";
        return 1;
    }

    if (yyparse() == 0 && astRoot)
    {
        std::cout << "Parsed successfully!\n";

        std::cout << "Running semantic analysis...\n";
        try
        {
            std::string resultType = astRoot->analyze(symbolTable);
            //std::cout << "Semantic analysis result: " << resultType << "\n";
            std::cout << "Semantic analysis completed successfully.\n";

            astRoot->print();

            std::cout << "Generating LLVM IR...\n";
            LLVMCodeGen llvmGen;
            llvmGen.generate(astRoot.get());
            llvmGen.dumpIR("output.ll");
            std::cout << "LLVM IR written to output.ll\n";
        }
        catch (const std::exception &e)
        {
            std::cerr << "Semantic error: " << e.what() << "\n";
        }
    }
    else
    {
        std::cerr << "Parsing failed.\n";
    }

    fclose(yyin);
    return 0;
}
