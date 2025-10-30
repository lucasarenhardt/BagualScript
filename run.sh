#!/bin/bash

set -e

# Compilação
bison -d bison.y
flex lexico.l
gcc -o analisador bison.tab.c lex.yy.c -lfl -g
g++ -o raposeitor raposeitor.cpp

# Execução
echo "Executando analisador..."
./analisador > teste.rap
echo "Executando raposeitor..."
./raposeitor teste.rap
