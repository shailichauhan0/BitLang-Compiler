# BitLang Compiler – Mini Compiler for a Custom Language
A custom mini-compiler project for BitLang — a beginner-friendly, minimal syntax programming language. Built using Flex, Bison, and C++, featuring full compiler pipeline including lexical analysis, parsing, AST generation, semantic checks, and planned LLVM-based code generation.


Team CD-VI-003 proudly presents **BitLang**, a beginner-friendly custom programming language, and its full-fledged compiler built using **Flex**, **Bison**, **C++**, and **LLVM**, with a fully integrated web-based frontend built in **React**. BitLang is designed to reduce the verbosity and complexity of traditional languages like C++ and Java, offering a clean and minimal syntax that helps both beginners and experienced programmers.

## 👥 Team Information

- **Team Name**: CD-VI-003 – BitLang  
- **Mentor**: Mr. Vikas Tripathi  

**Team Members**:
- **Shaili Chauhan** – 22022108  
- **Tvesa Gupta** – 220221227 
- **Aryan Parashar** – 22021519 

## 📌 Project Abstract

This project demonstrates the complete design and implementation of a custom language and its compiler. The compiler handles all essential stages including lexical analysis, parsing, semantic checks, and LLVM-based code generation. The end product is not only a command-line compiler but also includes a GUI frontend for enhanced usability and learning.

## 🧱 Architecture & Phases

- **Language Design**: Defined grammar and syntax supporting variables, arithmetic, conditionals, and loops.
- **Lexical Analysis**: Using Flex (`lexer.l`), converts source code into tokens.
- **Syntax Analysis**: Using Bison (`parser.y`), checks grammar and builds the AST.
- **Semantic Analysis**: Validates variable declarations, types, and scopes using symbol tables.
- **Intermediate Representation**: AST is converted to an intermediate format.
- **Code Generation**: LLVM is used to generate optimized low-level code.
- **GUI**: A web interface built in React to allow writing, compiling, and running BitLang programs visually.

## ✅ Tasks Completed

- BitLang syntax and grammar – Entire Team  
- Lexer implementation (Flex) – Aryan Parashar  
- Parser implementation (Bison) – Shaili Chauhan  
- AST logic – Tvesa Gupta  
- Symbol table and scope handling – Tvesa Gupta  
- AST integration and parsing – Shaili Chauhan  
- Error reporting – Aryan Parashar  
- LLVM IR generation and optimization – Aryan Parashar  
- Web GUI using React – Entire Team  
- Program testing and debugging – Entire Team  

## ⚠️ Challenges & Solutions

- **Grammar Conflicts**: Solved by defining operator precedence and rewriting ambiguous rules.
- **Symbol Table Management**: Built custom scope-aware table structures.
- **AST Design**: Iterated through multiple tree designs for flexibility.
- **Tool Integration**: Coordinated Flex/Bison with C++ and managed memory across modules.
- **Debugging**: Added custom debug helpers for AST, tokens, and symbol tables.
- **GUI Integration**: Managed asynchronous frontend-backend communication and response rendering.

## 📂 Project Structure

BitLang/
├── lexer.l  
├── parser.y  
├── main.cpp  
├── ast.*, SymbolTable.*  
├── llvm_codegen.*  
├── optimized.ll  
├── input.prog  
├── Makefile / CMakeLists.txt  
├── test_program/  
└── frontend/ (React App)

## 🛠️ How to Set Up and Run

### 🐧 Linux / WSL (Recommended)
```bash
sudo apt update
sudo apt install flex bison llvm clang cmake g++ python3
git clone https://github.com/TvesaDev3/NLP_based_command_prompt
cd NLP_based_command_prompt
make
./a.out input.prog
```

### 🪟 Windows (Using WinFlexBison and MinGW)
Install:
- WinFlexBison
- LLVM (Windows)
- CMake
- MinGW or TDM-GCC

Add all tools to **System PATH**, then run:
```powershell
win_bison -d parser.y -o parser.tab.cpp
win_flex -o lex.yy.cpp lexer.l
g++ -std=c++17 -o bitlang main.cpp ast.cpp SymbolTable.cpp parser.tab.cpp lex.yy.cpp llvm_codegen.cpp
.itlang.exe input.prog
```

## 💻 Run GUI (Frontend)

```bash
cd frontend
npm install
npm start
```

Open browser at `http://localhost:3000`

## 📄 Sample BitLang Program (`input.prog`)
```c
int a = 5;
int b = 10;
int sum = a + b;
print(sum);
```

## 📈 Project Outcome

- ✅ A fully working compiler from scratch
- ✅ Symbol table, AST, semantic checking
- ✅ LLVM-based backend for optimized IR
- ✅ Fully functional React-based web GUI
- ✅ Documentation, usage guide, and test cases

## 🧪 Testing Status

| Component             | Status | Notes                                  |
|-----------------------|--------|----------------------------------------|
| Lexer Tokenization    | ✅ Pass | All tokens identified correctly        |
| Parser Validation     | ✅ Pass | Valid parse tree and structure         |
| AST Construction      | ✅ Pass | Matches expected structure             |
| Semantic Checking     | ✅ Pass | Variable usage and type rules validated|
| LLVM IR Generation    | ✅ Pass | LLVM IR created and optimized          |
| Frontend Integration  | ✅ Pass | GUI communicates with backend          |

## 🔮 Future Scope

- Add support for functions, arrays, and user-defined types
- Better error handling with line/column indicators
- Object-oriented extensions like classes and inheritance
- Integrated debugger and runtime visualization
- More advanced LLVM optimization passes

## 🔗 GitHub Repository

**Repo Link**: https://github.com/TvesaDev3/NLP_based_command_prompt  
**Branch**: `main`  

### 🔑 Key Commits:
- `initial-lexer-flex` – Flex lexer setup  
- `parser-setup-bison` – Bison grammar rules  
- `ast-node-structure` – AST class implementation  
- `symbol-table-mapping` – Symbol table and scope validation  
- `llvm-ir-generation` – LLVM IR logic  
- `gui-integration` – React frontend linked with backend

## 📜 Conclusion

We successfully designed and implemented BitLang and its full compiler pipeline, including backend IR and frontend GUI. This project taught us core compiler principles, language design, memory and scope handling, and real-time user interaction using full-stack technologies.


