# BagualScript

## Como Usar

### Pré-requisitos
- Bison
- Flex
- GCC
- Biblioteca Flex (`libfl`)

## Compilação e Execução

Você pode compilar e executar o projeto de duas formas:

### 1. Execução automática (recomendada)

Use o script `run.sh` para compilar e executar tudo de uma vez:

```bash
chmod +x run.sh
./run.sh
```
O script irá compilar o analisador, o raposeitor e executar o código fonte (`teste.txt`).

### 2. Execução manual (passo a passo)

1. Gerar o analisador sintático com Bison:
	```bash
	bison -d bison.y
	```
2. Gerar o analisador léxico com Flex:
	```bash
	flex lexico.l
	```
3. Compilar o analisador:
	```bash
	gcc -o analisador bison.tab.c lex.yy.c -lfl -g
	```
4. Compilar o raposeitor:
	```bash
	g++ -o raposeitor raposeitor.cpp
	```
5. Executar o analisador e gerar o código intermediário:
	```bash
	./analisador > teste.rap
	```
6. Executar o raposeitor com o código gerado:
	```bash
	./raposeitor teste.rap
	```

### Nota
- O código fonte BagualScript deve estar em `teste.txt` (arquivo de entrada configurado no analisador)
- O arquivo `teste.rap` conterá o código intermediário gerado
