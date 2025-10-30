%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bison.tab.h" // nome que o bison gerar
%}

%option noyywrap

%%

"intche"              { return INT; }
"MOSTRE TCHE"         { return PRINT; }
"DIGA GURI"           { return SCAN; }
"SEGUE NA PELEIA"     { return WHILE; }
"CHEGA DE PELEIA"     { return BREAK; }
"BAH MAS SERA QUE"    { return IF; }
"CAPAZ"               { return ELSE; }
"FIM DA PICADA"       { return RETURN; }
"TEMO LACEANDO"		  { return FOR; }

"'"					  { return APOSTROF; }
"+"                   { return MAIS; }
"-"                   { return MENOS; }
"=="                  { return IGUAL; }
"="                   { return ATRIB; }
"!="                  { return DIFERENTE; }
"<"                   { return MENORQUE; }
">"                   { return MAIORQUE; }
"<="                  { return MENORIGUALQUE; }
">="                  { return MAIORIGUALQUE; }
"%"                   { return MOD; }
"("                   { return LPAR; }
")"                   { return RPAR; }
";"                   { return PEV; }
"/"                   { return DIV; }
"*"                   { return MULT; }
"&&"                  { return AND; }
"||"                  { return OR; }
"!"                   { return NOT; }
"++"                  { return MAISMAIS; }
"--"                  { return MENOSMENOS; }
"{"                   { return LCHAVES; }
"}"                   { return RCHAVES; }
"["                   { return LCOLCHETES; }
"]"                   { return RCOLCHETES; }
","                   { return VIRGULA; }
"\""[^"]*"\""    {yylval.string = strdup(yytext);return STRING;}  

-?[0-9]+              { yylval.inteiro = atoi(yytext); return NUM; }

[a-zA-Z][a-zA-Z0-9_]* { yylval.string = strdup(yytext); return ID; }

"//".* {}
" " {}
"\n" {}
"\t" {}
"\0" {}
"\a" {}
"\b" {}
"\f" {}
"\r" {}

.                      { fprintf(stderr, "ERRO LEXICO! Caractere inválido encontrado: %s\n", yytext);  return ERROR;}
%%

extern FILE *yyin;

/*
int main(int argc, char *argv[]) {
	if (argc > 1) {
		yyin = fopen(argv[1], "r");
		if (!yyin) { perror(argv[1]); return 1; }
	}

	yylex();

	if (yyin && yyin != stdin) fclose(yyin);
	return 0;
}
*/