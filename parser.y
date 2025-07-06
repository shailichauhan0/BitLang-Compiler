/* parser.y */
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#include <string>
#include <iostream>
#include <memory>
#include "ast.h"
#include "ast_interface.h"

extern "C" {
#endif
int yylex();
int yyparse();
void yyerror(const char *s);
int yywrap(void);

#ifdef __cplusplus
}

extern int yylineno;
extern std::unique_ptr<ASTNode> root;
#endif
%}

%define parse.error verbose
%locations

%code requires {
    #include "ast.h"
    #include "ast_interface.h"
}

%union {
    int ival;
    float fval;
    char cval;
    char* sval;
    int bval;
    ASTNode* node;
    BlockNode* block;
    DeclarationNode* declarationNodePtr;
    PrintStmtNode* printStmtNodePtr;
    IfStmtNode* ifStmtNodePtr;
    RepeatStmtNode* repeatStmtNodePtr;
    ReturnStmtNode* returnStmtNodePtr;
    std::vector<std::unique_ptr<ASTNode>>* stmtList;
}

%token <ival> INTEGER_LITERAL
%token <fval> FLOAT_LITERAL
%token <sval> STRING_LITERAL IDENTIFIER
%token <cval> CHAR_LITERAL
%token <bval> TRUE FALSE

%token INT FLOAT STRING BOOL
%token PRINT INPUT CLEAR TYPEOF RANDINT
%token IF ELSE REPEAT RETURN BREAK CONTINUE
%token PLUS MINUS STAR SLASH ASSIGN
%token EQ NEQ LEQ GEQ LT GT
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA
%token AND OR NOT
%token NEWLINE
%token UNKNOWN

%type <node> expression statement declaration print_stmt if_stmt repeat_stmt return_stmt assignment_stmt
%type <block> block
%type <stmtList> statement_list
%type <sval> type

%left OR
%left AND
%nonassoc EQ NEQ LT GT LEQ GEQ
%left PLUS MINUS
%left STAR SLASH
%right NOT

%%

program:
    program statement          { addToProgram(std::unique_ptr<ASTNode>($2)); }
  | /* empty */                 { astRoot = makeProgram(); }
  ;

statement:
    declaration end             { $$ = $1; }
  | assignment_stmt end        { $$ = $1; }
  | print_stmt end             { $$ = $1; }
  | if_stmt                    { $$ = $1; }
  | repeat_stmt                { $$ = $1; }
  | return_stmt end            { $$ = $1; }
  | BREAK end                  { $$ = makeBreak(@1.first_line).release(); } 
  | CONTINUE end               { $$ = makeContinue(@1.first_line).release(); }
  | NEWLINE                    { $$ = nullptr; }
  ;

statement_list:
    statement_list statement {
        if ($2) $1->push_back(std::unique_ptr<ASTNode>($2));
        $$ = $1;
    }
  | /* empty */ {
        $$ = new std::vector<std::unique_ptr<ASTNode>>();
    }
  ;

declaration:
    type IDENTIFIER ASSIGN expression { $$ = makeDeclaration($1, $2, std::unique_ptr<ASTNode>($4), @2.first_line).release(); }
  ;

type:
    INT                         { $$ = strdup("int"); }
  | FLOAT                       { $$ = strdup("float"); }
  | STRING                      { $$ = strdup("string"); }
  | BOOL                        { $$ = strdup("bool"); }
  ;

print_stmt:
    PRINT LPAREN expression RPAREN { $$ = makePrintStmt(std::unique_ptr<ASTNode>($3), @1.first_line).release(); }
  ;

if_stmt:
    IF LPAREN expression RPAREN block              { $$ = makeIfStmt(std::unique_ptr<ASTNode>($3), std::unique_ptr<BlockNode>($5), nullptr, @1.first_line).release(); }
  | IF LPAREN expression RPAREN block ELSE block   { $$ = makeIfStmt(std::unique_ptr<ASTNode>($3), std::unique_ptr<BlockNode>($5), std::unique_ptr<BlockNode>($7), @1.first_line).release(); }
  ;

repeat_stmt:
    REPEAT LPAREN expression RPAREN block          { $$ = makeRepeatStmt(std::unique_ptr<ASTNode>($3), std::unique_ptr<BlockNode>($5) ,@1.first_line).release(); }
  ;

return_stmt:
    RETURN expression                              { $$ = makeReturnStmt(std::unique_ptr<ASTNode>($2) ,@1.first_line).release(); }
  ;

assignment_stmt:
    IDENTIFIER ASSIGN expression {
        $$ = makeAssignment($1, std::unique_ptr<ASTNode>($3) ,@1.first_line).release();
    }
  ;

block:
    LBRACE statement_list RBRACE {
        $$ = makeBlock(std::unique_ptr<std::vector<std::unique_ptr<ASTNode>>>($2) ,@1.first_line).release();
    }
  ;

expression:
    INTEGER_LITERAL              { $$ = makeIntLiteral($1, @1.first_line).release(); }
  | FLOAT_LITERAL                { $$ = makeFloatLiteral($1, @1.first_line).release(); }
  | STRING_LITERAL               { $$ = makeStringLiteral($1, @1.first_line).release(); }
  | CHAR_LITERAL                 { $$ = makeCharLiteral($1, @1.first_line).release(); }
  | TRUE                         { $$ = makeBoolLiteral(true, @1.first_line).release(); }
  | FALSE                        { $$ = makeBoolLiteral(false, @1.first_line).release(); }
  | IDENTIFIER                   { $$ = makeIdentifier($1, @1.first_line).release(); }
  | expression PLUS expression   { $$ = makeBinaryExpr(std::unique_ptr<ASTNode>($1), BinaryExprNode::Op::Add, std::unique_ptr<ASTNode>($3), @2.first_line).release(); }
  | expression MINUS expression  { $$ = makeBinaryExpr(std::unique_ptr<ASTNode>($1), BinaryExprNode::Op::Sub, std::unique_ptr<ASTNode>($3), @2.first_line).release(); }
  | expression STAR expression   { $$ = makeBinaryExpr(std::unique_ptr<ASTNode>($1), BinaryExprNode::Op::Mul, std::unique_ptr<ASTNode>($3), @2.first_line).release(); }
  | expression SLASH expression  { $$ = makeBinaryExpr(std::unique_ptr<ASTNode>($1), BinaryExprNode::Op::Div, std::unique_ptr<ASTNode>($3), @2.first_line).release(); }
  | LPAREN expression RPAREN     { $$ = $2; }
  | expression EQ expression     { $$ = makeBinaryExpr(std::unique_ptr<ASTNode>($1), BinaryExprNode::Op::Eq, std::unique_ptr<ASTNode>($3), @2.first_line).release(); }
  | expression NEQ expression    { $$ = makeBinaryExpr(std::unique_ptr<ASTNode>($1), BinaryExprNode::Op::Neq, std::unique_ptr<ASTNode>($3), @2.first_line).release(); }
  | expression LT expression     { $$ = makeBinaryExpr(std::unique_ptr<ASTNode>($1), BinaryExprNode::Op::Lt, std::unique_ptr<ASTNode>($3), @2.first_line).release(); }
  | expression GT expression     { $$ = makeBinaryExpr(std::unique_ptr<ASTNode>($1), BinaryExprNode::Op::Gt, std::unique_ptr<ASTNode>($3), @2.first_line).release(); }
  | expression LEQ expression    { $$ = makeBinaryExpr(std::unique_ptr<ASTNode>($1), BinaryExprNode::Op::Leq, std::unique_ptr<ASTNode>($3), @2.first_line).release(); }
  | expression GEQ expression    { $$ = makeBinaryExpr(std::unique_ptr<ASTNode>($1), BinaryExprNode::Op::Geq, std::unique_ptr<ASTNode>($3), @2.first_line).release(); }
  | expression AND expression    { $$ = makeBinaryExpr(std::unique_ptr<ASTNode>($1), BinaryExprNode::Op::And, std::unique_ptr<ASTNode>($3), @2.first_line).release(); }
  | expression OR expression     { $$ = makeBinaryExpr(std::unique_ptr<ASTNode>($1), BinaryExprNode::Op::Or, std::unique_ptr<ASTNode>($3), @2.first_line).release(); }
  | NOT expression               { $$ = makeUnaryExpr(UnaryExprNode::Op::Not, std::unique_ptr<ASTNode>($2), @1.first_line).release(); }
  | MINUS expression             { $$ = makeUnaryExpr(UnaryExprNode::Op::Minus, std::unique_ptr<ASTNode>($2), @1.first_line).release(); }
  | INPUT LPAREN RPAREN          { $$ = makeBuiltinCall("input", {}, @1.first_line).release(); }
  ;

end:
    SEMICOLON
  | NEWLINE
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s at line %d\n", s, yylineno);
}
