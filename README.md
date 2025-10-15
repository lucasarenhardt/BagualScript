# BagualScript

## Como Usar

### Pré-requisitos
- Bison
- Flex
- GCC
- Biblioteca Flex (`libfl`)

### Compilação

1. Gerar o analisador sintático com Bison:
```bash
bison -d sintatico.y
```

2. Gerar o analisador léxico com Flex:
```bash
flex lexico.l
```

3. Compilar o analisador:
```bash
gcc -o analisador sintatico.tab.c lex.yy.c -lfl -g
```

4. Compilar o raposeitor (se necessário):
```bash
g++ -o raposeitor raposeitor.cpp
```

### Execução

1. Executar o analisador e gerar o código intermediário:
```bash
./analisador > teste.rap
```

2. Executar o raposeitor com o código gerado:
```bash
./raposeitor teste.rap
```

### Fluxo Completo

```bash
# Compilar tudo
bison -d sintatico.y
flex lexico.l
gcc -o analisador sintatico.tab.c lex.yy.c -lfl -g
g++ -o raposeitor raposeitor.cpp

# Executar
./analisador > teste.rap
./raposeitor teste.rap
```

### Nota
- O código fonte BagualScript deve estar em `teste.txt` (arquivo de entrada configurado no analisador)
- O arquivo `teste.rap` conterá o código intermediário gerado
