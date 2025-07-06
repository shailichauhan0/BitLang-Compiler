/* lexer.l */
%{
    #ifdef __cplusplus
extern "C" {
#endif
    int yylex(void);
#ifdef __cplusplus
}
#endif

#include "parser.tab.h"
#include <string.h>
#include <stdlib.h>

extern YYSTYPE yylval;


extern int yylineno;
extern YYLTYPE yylloc;


char* translateString(char* str, int size);

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno;

%}

%option noyywrap
%option yylineno


%%
"int"       return INT;
"float"     return FLOAT;
"string"    return STRING;
"bool"      return BOOL;
"true"      { yylval.bval = 1; return TRUE; }
"false"     { yylval.bval = 0; return FALSE; }
"print"     return PRINT;
"input"     return INPUT;
"clear"     return CLEAR;
"typeof"    return TYPEOF;
"randint"   return RANDINT;
"if"        return IF;
"else"      return ELSE;
"repeat"    return REPEAT;
"return"    return RETURN;
"stop"      return BREAK;
"skip"      return CONTINUE;

"and"       return AND;
"or"        return OR;
"not"       return NOT;

[0-9]+\.[0-9]+   { yylval.fval = atof(yytext); return FLOAT_LITERAL; }
[0-9]+           { yylval.ival = atoi(yytext); return INTEGER_LITERAL; }
"=="            return EQ;
"!="            return NEQ;
"<="            return LEQ;
">="            return GEQ;
"<"             return LT;
">"             return GT;
"="             return ASSIGN;
"+"             return PLUS;
"-"             return MINUS;
"*"             return STAR;
"/"             return SLASH;
"("             return LPAREN;
")"             return RPAREN;
"{"             return LBRACE;
"}"             return RBRACE;
";"             return SEMICOLON;
","             return COMMA;

\'([^\\]|\\.)\' { yylval.cval = yytext[1]; return CHAR_LITERAL; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = strdup(yytext); return IDENTIFIER; }
\"([^\\\"]|\\.)*\" { yylval.sval = translateString(yytext + 1, yyleng - 2); return STRING_LITERAL; }
[ \t\r]+    ;
\n+         { return NEWLINE; }
.            return UNKNOWN;
%%



char* translateString(char* str, int size) {
    char* newString = (char*) malloc(size + 1);
    char* temp = newString;
    int i = 0;
    while (i < size) {
        if (*str == '\\') {
            str++; i++;
            switch (*str) {
                case 'n': *temp = '\n'; break;
                case 't': *temp = '\t'; break;
                case 'r': *temp = '\r'; break;
                case '"': *temp = '"'; break;
                case '\\': *temp = '\\'; break;
                case '\'': *temp = '\''; break;
                default: *temp = *str; break;
            }
        } else {
            *temp = *str;
        }
        str++; temp++; i++;
    }
    *temp = '\0';
    return newString;
}
