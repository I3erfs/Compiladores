%{
#include "parser.tab.h"
#include "semantic/src/semantic_analysis.h"
#include <stdio.h>
void ignorar_comentario();
extern int yylineno; // Variável global de linha
extern FILE *input_file; // Adicione esta linha para declarar input_file

#define CPY_TK strncpy(token_str, yytext, TOKEN_LEN);
%}

%option yylineno

%%
"if"          { return IF; }
"else"        { return ELSE; }
"while"       { return WHILE; }
"return"      { return RETURN; }
"int"         { return INT; }
"void"        { return VOID; }
"=="          { return EQ; }
"!="          { return NEQ; }
"<"           { return LT; }
"<="          { return LTE; }
">"           { return GT; }
">="          { return GTE; }
"="           { return ASSIGN; }
"+"           { return PLUS; }
"-"           { return MINUS; }
"*"           { return TIMES; }
"/"           { return DIV; }
";"           { return SEMICOLON; }
","           { return COMMA; }
"("           { return LPAREN; }
")"           { return RPAREN; }
"{"           { return LBRACE; }
"}"           { return RBRACE; }
"["           { return LBRACK; }
"]"           { return RBRACK; }
"/*"          { ignorar_comentario(); }

[0-9]+        { return NUM; }
[a-zA-Z_][a-zA-Z0-9_]* { return ID; }

[ \t\r]+    { /* Ignorar espaços e quebras de linha */ }

\n            { yylineno++; }

<<EOF>>  { return 0; }

.             { fprintf(stderr, "'ERRO LÉXICO: %s' 'LINHA: %d'\n", yytext, yylineno - 1); }
%%

// Função para ignorar comentários
void ignorar_comentario() {
    int c;
    while (1) {
        c = input();
        if (c =='*'){
            c = input();
            if (c=='/') break;
        }
    }
}

yytoken_kind_t getToken() {
    
	static int first_time = 1;
    yytoken_kind_t current_token;

    if (first_time) {
    	
        first_time = 0;
        yylineno++;
        yyin = input_file;
        yyout = stdout;
    }

    current_token = yylex();
    return current_token;
}