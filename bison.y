%{
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
int capturaEnd(char *nome);

typedef enum {
    ID_VARIAVEL_SIMPLES,
    ID_VETOR
} TipoID;

typedef struct { // guarda as declaracoes de variavel
    char *nome; // nome da variavel
    int endereco; // endereco na memoria simulada
    TipoID tipo; // Identificar se é var ou vetor
} entrada;

// declaracao de variaveis necessarias para o cpl durante a analise

entrada tabEntrada[25000]; // armazena todas as variaveis declaradas no codigo

int idx_entradas = 0; // contador para o nmr de entradas (variaveis) atualmente na tabela
int end = 0; // contador para simular o prox endereco de memoria livre a ser alocado

int pilhaCond[300]; // pilha para gerenciar desvio (lacos e condicionais)
int topoCond = 0;

int pilhaLoop[300]; // pilha para gerenciar BREAK (só loops)
int topoLoop = 0;

int nmr_jumpFalse = 0; // guarda o nmr do rotulo para onde um comando "salto se falso" deve ir
int t = 0; // contador para os registradores temporarios
int desvio = 0; // contador para os rotulo de desvio

int dest_else[150]; // vetor para guardar o destino de um else (para gerar salto correto)
int idx_else = 0; //proxima posicao livre

// FUNCOES AUXILIARES

// FUNCAO SEMANTICA: Verifica declaracao
void verificaDeclaracao(char *nome) {
    if (capturaEnd(nome) != -1) {
        fprintf(stderr, "ERRO SEMANTICO: Variavel '%s' ja foi declarada\n", nome);
        exit(1); // Interrompe a compilação
    }
}

// FUNCAO SEMANTICA: Verifica se é uma variavel simples declarada
int buscaEValidaVariavelSimples(char *nome) {
    for (int i = 0; i < idx_entradas; i++) {
        if (strcmp(tabEntrada[i].nome, nome) == 0) { // Achou o nome
            // VERIFICACAO DE TIPO:
            if (tabEntrada[i].tipo != ID_VARIAVEL_SIMPLES) {
                fprintf(stderr, "ERRO SEMANTICO: '%s' é um vetor e foi usado como variavel simples.\n", nome);
                exit(1);
            }
            return tabEntrada[i].endereco; 
        }
    }
    // Se saiu do loop n achou
    fprintf(stderr, "ERRO SEMANTICO: Variavel '%s' nao foi declarada\n", nome);
    exit(1);
    return -1;
}

// FUNCAO SEMANTICA: Verifia se é um vetor declarado
int buscaEValidaVetor(char *nome) {
    for (int i = 0; i < idx_entradas; i++) {
        if (strcmp(tabEntrada[i].nome, nome) == 0) { // Achou o nome
            // VERIFICACAO DE TIPO:
            if (tabEntrada[i].tipo != ID_VETOR) {
                fprintf(stderr, "ERRO SEMANTICO: '%s' é uma variavel simples e foi usada como vetor.\n", nome);
                exit(1);
            }
            return tabEntrada[i].endereco; 
        }
    }
    // Se saiu do loop n achou
    fprintf(stderr, "ERRO SEMANTICO: Vetor '%s' nao foi declarado\n", nome);
    exit(1);
    return -1;
}


int capturaEnd(char *nome) {
    for (int i = 0; i < idx_entradas; i++) {
        int retorno = strcmp(tabEntrada[i].nome, nome); // se o nome for igual o da tabela retorna 0
        if (!retorno) { // encontrado
            return tabEntrada[i].endereco;
        }
    }
    return -1; // caso n encontre
}

// empilha x na pilhaCOND
void pushPilhaCOND(int x) {
    if (topoCond >= 300) { // verifica se a pilha esta cheia
        printf("Pilha de desvios condicionais e lacos cheia\n");
        return;
    }
    // coloca x no topo e incrementa o topo
    pilhaCond[topoCond] = x;
    topoCond++;
}

// desempilha da pilha COND
int popPilhaCOND() {
    if (topoCond <= 0) { // verifica se não ta vazia
        printf("Pilha de desvios condicionais e lacos vazia\n");
        return 0;
    }
    topoCond--;
    return pilhaCond[topoCond];
}

// empilha x na pilhaLoop (para BREAK)
void pushPilhaLoop(int x) {
    if (topoLoop >= 300) {
        printf("Pilha de loops cheia\n");
        return;
    }
    pilhaLoop[topoLoop] = x;
    topoLoop++;
}

// desempilha da pilha Loop
int popPilhaLoop() {
    if (topoLoop <= 0) {
        printf("Pilha de loops vazia\n");
        return 0;
    }
    topoLoop--;
    return pilhaLoop[topoLoop];
}

%}

// tipos de dados que vao ser associados a um token ou regra
%union {
    char *string;
    int inteiro;
}

// tokens que carregam um tipo string
%token <string> ID STRING
// tokens tipo inteiro
%token <inteiro> NUM
// tokens que são simbolos e nao possuem valor extra
%token PEV FOR WHILE IF ELSE INT PRINT VIRGULA SCAN ERROR ATRIB
%token MAISMAIS MENOSMENOS MENORQUE MAIORQUE IGUAL DIFERENTE MAIORIGUALQUE MENORIGUALQUE
%token MAIS MENOS LPAR RPAR RCOLCHETES LCOLCHETES LCHAVES RCHAVES MULT MOD AND NOT OR DIV BREAK APOSTROF

// tiposS de retorno 
%type <inteiro> expressao term expressao1 logicas logica_and logica_not logica_base
%type <string> oper increm forVar lacoFor parteIncrem

%define parse.error verbose

// Definicoes de precedencia para remover ambiguidades logicas
%left OR
%left AND
%right NOT

%nonassoc "then" // Marcador para um IF sem ELSE
%nonassoc ELSE // IF ... ELSE

%%
// regras gramaticais

// regra inicial para ler o programa q é uma sequência de comandos
codigo: atrib codigo
      | lacoFor codigo
      | lacoWhile codigo
      | exibir codigo
      | If codigo
      | pegarEntrada codigo
      | BREAK PEV { printf("jump R0%d\n", pilhaLoop[topoLoop - 1]); }
      | ERROR
      | ;

// regra para scan de valores do teclado
pegarEntrada: SCAN LPAR ID RPAR {
                int endereco_var = buscaEValidaVariavelSimples($3);
                printf("read %%r%d\n", endereco_var);
            } PEV
            | SCAN LPAR ID LCOLCHETES expressao {
                int endereco_var = buscaEValidaVetor($3);
                printf("read %%t%d\nstore %%t%d, %%t%d(%d)\n", t, t, $5, endereco_var);
                t++;
            } RCOLCHETES RPAR PEV;

// regra para o comando de impressao
exibir: PRINT LPAR print RPAR PEV ;

// regra para os argumentos do comando de impressao
print: expressao { printf("printv %%t%d \n", $1); } VIRGULA print
     | STRING { printf("printf %s\n", $1); } VIRGULA print
     | expressao { printf("printv %%t%d \n", $1); }
     | STRING { printf("printf %s\n", $1); };

// regra para atribuicoes e declaracoes de variaveis
atrib: INT ID ATRIB expressao PEV {
           // verificacao semantica (declaracao duplicada)
           verificaDeclaracao($2);

           // acao na tabela (add com o tipo simples)
           tabEntrada[idx_entradas] = (entrada){$2, end, ID_VARIAVEL_SIMPLES};

           // gera codigo intermediario
           printf("mov %%r%d, %%t%d\n", end, $4);
           idx_entradas++;
           end++;
       }
     | INT ID PEV {
           // verificiacao semantica (declaracao duplicada)
           verificaDeclaracao($2);

           // acao na tabela (add com o tipo simples)
           tabEntrada[idx_entradas] = (entrada){$2, end, ID_VARIAVEL_SIMPLES};
           idx_entradas++;
           end++;
       }
     | ID ATRIB expressao PEV { // Ex: x = 5;
           // verificacao semantica (uso de variavel simples)
           // verifica se $1 existe e eh uma variavel simples
           int endereco_var = buscaEValidaVariavelSimples($1);

           // gera codigo intermediario
           printf("mov %%r%d, %%t%d\n", endereco_var, $3);
       }
     | INT ID LCOLCHETES NUM {
           // verificacao semantica (declaracao duplicada)
           verificaDeclaracao($2);

           // acao na tabela (add com o tipo vetor)
           tabEntrada[idx_entradas] = (entrada){$2, end, ID_VETOR};
           idx_entradas++;
           end += $4;
       }
       RCOLCHETES PEV
     | ID LCOLCHETES expressao RCOLCHETES ATRIB expressao PEV {
           // ve se $1 existe e eh um vetor.
           int endereco_var = buscaEValidaVetor($1);

           // $3 é o índice, $6 é o valor
           printf("store %%t%d, %%t%d(%d)\n", $6, $3, endereco_var);
       }
     ;

// regra principal para a estrutura do for
lacoFor: FOR LPAR atrib {
             nmr_jumpFalse = desvio + 1;
             printf("label R0%d\n", desvio);
             pushPilhaCOND(desvio++);
             pushPilhaCOND(desvio);
             pushPilhaLoop(desvio++);
         } forVar PEV parteIncrem RPAR LCHAVES codigo RCHAVES {
             int R0_end, R0_cond;
             int endereco_var = buscaEValidaVariavelSimples($7);
             printf("add %%r%d, %%r%d, 1\n", endereco_var, endereco_var);
             popPilhaLoop();
             R0_end = popPilhaCOND();
             R0_cond = popPilhaCOND();
             printf("jump R0%d\nlabel R0%d\n", R0_cond, R0_end);
         };

// regra para a condição de parada do for
forVar: ID oper expressao {
            int endereco_var = buscaEValidaVariavelSimples($1);
            printf("%s %%t%d, %%r%d, %%t%d\njf %%t%d, R0%d\n", $2, t, endereco_var, $3, t, nmr_jumpFalse);
            $$ = $1;
            t++;
        };

// regra para a parte de incremento do for
parteIncrem: increm ID { $$ = $2; }
           | ID increm { $$ = $1; };

// regra para o while
lacoWhile: WHILE {
               nmr_jumpFalse = desvio + 1;
               printf("label R0%d \n", desvio);
               pushPilhaCOND(desvio++);
               pushPilhaCOND(desvio);
               pushPilhaLoop(desvio++);
           } LPAR logicas {
               printf("jf %%t%d, R0%d\n", $4, nmr_jumpFalse);
           } RPAR LCHAVES codigo RCHAVES {
               int R0_end, R0_cond;
               popPilhaLoop();
               R0_end = popPilhaCOND();
               R0_cond = popPilhaCOND();
               printf("jump R0%d\nlabel R0%d\n", R0_cond, R0_end);
           };

// regra para o if
If: IF {
        nmr_jumpFalse = desvio;
        pushPilhaCOND(desvio++);
        pushPilhaCOND(desvio++);
    } LPAR logicas {
        printf("jf %%t%d, R0%d\n", $4, nmr_jumpFalse);
    } RPAR LCHAVES codigo RCHAVES else ;

// regra para else
else: ELSE {
          dest_else[idx_else++] = popPilhaCOND();
          printf("jump R0%d\n", dest_else[idx_else - 1]);
          printf("label R0%d\n", popPilhaCOND());
      } LCHAVES codigo RCHAVES {
          printf("label R0%d\n", dest_else[--idx_else]);
      }
    | { // caso IF sem ELSE
          int x = popPilhaCOND();
          printf("jump R0%d\n", x);
          printf("label R0%d\nlabel R0%d\n", popPilhaCOND(), x);
      } %prec "then"
    ;

// REGRAS LOGICAS

logicas: logicas OR logica_and {
             printf("or %%t%d, %%t%d, %%t%d \n", t, $1, $3);
             $$ = t;
             t++;
         }
       | logica_and { $$ = $1; }
       ;

logica_and: logica_and AND logica_not {
                printf("and %%t%d, %%t%d, %%t%d \n", t, $1, $3);
                $$ = t;
                t++;
            }
          | logica_not { $$ = $1; }
          ;

logica_not: NOT logica_not {
                printf("not %%t%d, %%t%d\n", t, $2);
                $$ = t;
                t++;
            } %prec NOT
          | logica_base { $$ = $1; }
          ;

logica_base: expressao oper expressao { // uma comparacao simples
                 printf("%s %%t%d, %%t%d, %%t%d\n", $2, t, $1, $3);
                 $$ = t;
                 t++;
             }
           | LPAR logicas RPAR { // uma expr entre parenteses
                 $$ = $2;
             }
           ;

// regra que converte tokens de operadores de comparacao em strings
oper: MAIORQUE { $$ = "greater"; }
    | MENORQUE { $$ = "less"; }
    | IGUAL { $$ = "equal"; }
    | DIFERENTE { $$ = "diff"; }
    | MENORIGUALQUE { $$ = "lesseq"; }
    | MAIORIGUALQUE { $$ = "greatereq"; };

// regra para os operadores de incremento/decremento
increm: MAISMAIS { $$ = "add"; }
      | MENOSMENOS { $$ = "sub"; };

// regras para expressoes aritmeticas, definindo a precedencia
expressao: expressao MAIS expressao1 {
               printf("add %%t%d, %%t%d, %%t%d\n", t, $1, $3);
               $$ = t++;
           }
         | expressao MENOS expressao1 {
               printf("sub %%t%d, %%t%d, %%t%d\n", t, $1, $3);
               $$ = t++;
           }
         | expressao1 { $$ = $1; };

expressao1: expressao1 DIV term {
                printf("div %%t%d, %%t%d, %%t%d\n", t, $1, $3);
                $$ = t++;
            }
          | expressao1 MULT term {
                printf("mult %%t%d, %%t%d, %%t%d\n", t, $1, $3);
                $$ = t++;
            }
          | expressao1 MOD term {
                printf("mod %%t%d, %%t%d, %%t%d\n", t, $1, $3);
                $$ = t++;
            }
          | term { $$ = $1; };

// regra para os termos de uma expressao (variaveis, numeros...)
term: ID {
          int endereco_var = buscaEValidaVariavelSimples($1);
          printf("mov %%t%d, %%r%d\n", t, endereco_var);
          $$ = t++;
      }
    | NUM {
          printf("mov %%t%d, %d\n", t, $1);
          $$ = t++;
      }
    | LPAR expressao RPAR { $$ = $2; }
    | MENOS ID {
          int endereco_var = buscaEValidaVariavelSimples($2);
          printf("mov %%t%d, %%r%d\n", t, endereco_var);
          $$ = t++;
      }
    | MENOS NUM {
          printf("mov %%t%d, %d\n", t, -$2);
          $$ = t++;
      }
    | ID LCOLCHETES expressao RCOLCHETES {
          int endereco_var = buscaEValidaVetor($1);
          printf("load %%t%d, %%t%d(%d) \n", t, $3, endereco_var);
          $$ = t++;
      }
    ;

%%
//rotinas de suporte

//arquivo de entrada usada pelo lexico
extern FILE *yyin;

int main(int argc, char *argv[]) {
    // abre o arquivo "teste.txt" em modo de leitura e o associa a `yyin`.
    yyin = fopen("teste.txt", "r");

    // chama a funcao principal gerada pelo Bison para iniciar a analise sintatica.
    yyparse();

    // fecha o arquivo apos a analise
    fclose(yyin);

    return 0;
}

// funcao de tratamento de erro
void yyerror(const char *s) {
    fprintf(stderr, "ERRO SINTATICO! %s\n", s);
}