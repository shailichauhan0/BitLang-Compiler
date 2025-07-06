CXX = clang++
CXXFLAGS = -std=c++17 `llvm-config --cxxflags` -fexceptions
LDFLAGS = `llvm-config --ldflags --system-libs --libs core`

LEX = flex
YACC = bison

LEX_SRC = lexer.l
YACC_SRC = parser.y

LEX_GEN = lex.yy.c
YACC_GEN_C = parser.tab.c
YACC_GEN_H = parser.tab.h

OBJS = main.o ast.o SymbolTable.o llvm_codegen.o parser.tab.o lex.yy.o

TARGET = compiler

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.cpp llvm_codegen.h
	$(CXX) $(CXXFLAGS) -c main.cpp

ast.o: ast.cpp ast.h ast_interface.h
	$(CXX) $(CXXFLAGS) -c ast.cpp

llvm_codegen.o: llvm_codegen.cpp llvm_codegen.h
	$(CXX) $(CXXFLAGS) -c llvm_codegen.cpp

SymbolTable.o: SymbolTable.cpp SymbolTable.h
	$(CXX) $(CXXFLAGS) -c SymbolTable.cpp

parser.tab.o: $(YACC_GEN_C)
	$(CXX) $(CXXFLAGS) -c $(YACC_GEN_C)

lex.yy.o: $(LEX_GEN)
	$(CXX) $(CXXFLAGS) -c $(LEX_GEN)

$(LEX_GEN): $(LEX_SRC)
	$(LEX) $(LEX_SRC)

$(YACC_GEN_C) $(YACC_GEN_H): $(YACC_SRC)
	$(YACC) -d $(YACC_SRC)

clean:
	rm -f *.o $(TARGET) $(LEX_GEN) $(YACC_GEN_C) $(YACC_GEN_H) output.ll
