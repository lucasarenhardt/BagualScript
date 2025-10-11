%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%option noyywrap

%%

"intche"              { printf("<INT> "); }
"MOSTRE TCHE"         { printf("<PRINT> "); }
"DIGA GURI"           { printf("<SCAN> "); }
"SEGUE NA PELEIA"     { printf("<WHILE> "); }
"CHEGA DE PELEIA"     { printf("<BREAK> "); }
"BAH MAS SERA QUE"    { printf("<IF> "); }
"CAPAZ"               { printf("<ELSE> "); }
"FIM DA PICADA"       { printf("<RETURN> "); }

"+"                   { printf("<MAIS> "); }
"-"                   { printf("<MENOS> "); }
"=="                  { printf("<IGUAL> "); }
"="                   { printf("<ATRIB> "); }
"!="                  { printf("<DIFERENTE> "); }
"<"                   { printf("<MENORQUE> "); }
">"                   { printf("<MAIORQUE> "); }
"<="                  { printf("<MENORIGUALQUE> "); }
">="                  { printf("<MAIORIGUALQUE> "); }
"%"                   { printf("<MOD> "); }
"("                   { printf("<LPAR> "); }
")"                   { printf("<RPAR> "); }
";"                   { printf("<PEV> "); }
"/"                   { printf("<DIV> "); }
"*"                   { printf("<MULT> "); }
"&&"                  { printf("<AND> "); }
"||"                  { printf("<OR> "); }
"!"                   { printf("<NOT> "); }
"++"                  { printf("<MAISMAIS> "); }
"--"                  { printf("<MENOSMENOS> "); }
"{"                   { printf("<RCHAVES> "); }
"}"                   { printf("<LCHAVES> "); }
"["                   { printf("<RCOLCHETES> "); }
"]"                   { printf("<LCOLCHETES> "); }
","                   { printf("<VIRGULA> "); }
"\""                  { printf("<ASPASDUPLAS> "); }

[0-9]+                { printf("<NUM: \"%s\"> ", yytext); }
-?[0-9]+              { printf("<NUM: \"%s\"> ", yytext); }

[a-zA-Z][a-zA-Z0-9_]* { printf("<ID: \"%s\"> ", yytext); }

"//".*"\n" { }
" " {}
"\t" {}
"\n" {}
"\0" {}
"\a" {}
"\b" {}
"\f" {}
"\r" {}

.                      { fprintf(stderr, "Caractere inválido encontrado %s\n", yytext); }
%%

extern FILE *yyin;

int main(int argc, char *argv[]) {
	if (argc > 1) {
		yyin = fopen(argv[1], "r");
		if (!yyin) { perror(argv[1]); return 1; }
	}

	yylex();

	if (yyin && yyin != stdin) fclose(yyin);
	return 0;
}
