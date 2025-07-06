cmake_minimum_required(VERSION 3.16)
project(OurMiniCompiler LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Flex and Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM libraries: ${LLVM_LIBS}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Flex/Bison files
set(LEX_FILE ${CMAKE_SOURCE_DIR}/lexer.l)
set(YACC_FILE ${CMAKE_SOURCE_DIR}/parser.y)

BISON_TARGET(MyParser ${YACC_FILE} ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h
    COMPILE_FLAGS --defines=${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h)

FLEX_TARGET(MyLexer ${LEX_FILE} ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cpp)
ADD_FLEX_BISON_DEPENDENCY(MyLexer MyParser)

# Source files
set(SOURCES
    main.cpp
    ast.cpp
    SymbolTable.cpp
    llvm_codegen.cpp
    ${BISON_MyParser_OUTPUTS}
    ${FLEX_MyLexer_OUTPUTS}
)

# Compiler binary
add_executable(compiler ${SOURCES})

target_include_directories(compiler PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

llvm_map_components_to_libnames(llvm_libs core support)

target_link_libraries(compiler PRIVATE ${llvm_libs})
target_compile_options(compiler PRIVATE ${LLVM_CXX_FLAGS})

# === Custom target to run the full pipeline ===
add_custom_target(run ALL
    COMMAND ./compiler test.prog
    COMMAND opt-17 -O2 output.ll -o optimized.ll
    COMMAND lli-17 optimized.ll
    DEPENDS compiler
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running compiler → optimizing IR → running optimized code"
)
