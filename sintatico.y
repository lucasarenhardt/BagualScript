%{
#include <string.h>
#include <stdio.h>

typedef struct{ // auxilia em expressoes logicas complexas (AND/OR)
    char *operador; // AND/OR
    // registradores temporarios (r3 = r3(resultado de outra operacao)) OR r2(result. de outra operacao))
    int r1;
    int r2;
    int r3;
}logicas;

typedef struct{ // guarda as declaracoes de variavel
    char *nome; // nome da variavel 
    char endereco;// endereco na memoria simulada
}entrada;


// delcaracao de variaveis necessarias para o cpl durante a analise

entrada tabEntrada[25000]; // armazena todas as variaveis declaradas no codigo
logicas tabLogicas[1000]; // armazena as expressoes logicas do codigo

int idx_entradas = 0; // contador para o nmr de entradas (variaveis) atualmente na tabela
int end = 0; // contador para simular o prox endereco de memoria livre a ser alocado
int idx_logicas = 0; // indice para a prox pos livre no vetor de 'logicas'

int cplx = -1; // flag para indicar se uma logica complexa (AND/OR) esta sendo analisada

int pilhaCond[300]; // pilha paar gerenciar desvio (lacos e condicionais)
int topoCond = 0;

int pilhaPar[300]; // pilha para ordem de avaliacao de expressoes com "()"
int topoPar = 0;

int nmr_jumpFalse = 0; // guarda o nmr do rotulo para onde um comando "salto se falso" deve ir
int t = 0; // contador para os registradores temporarios
int desvio = 0; // contador para os rotulo de desvio

int dest_else[150]; // vetor para guardar o destino de um else (para gerar salto correto)
int idx_else = 0; //proxima posicao livre

// FUNÇÔES AUXILIARES

void popLogicas(int r1){ // gera o codigo para um op logica (AND/OR)
    //imprime a instrucao
    printf("%s %%t%d, %%t%d, %%t%d \n", tabLogicas[idx_logicas].operador, r1+1, tabLogicas[idx_logicas].r2, r1);
}

int capturaEnd(char *nome){
    for(int i=0; i<idx_entradas; i++){
        int retorno = strcmp(tabEntrada[i].nome, nome); // se o nome for igual o da tabela retorna 0
        if(!retorno){ // encontrado 
            return tabEntrada[i].endereco;
        }
    }
    return -1; // caso nao encontre
}

// coloca um nmr de registrador na pilha auxiliar para ordem de expressoes com "()"
void pushPilhaPAR(int x){
    if(topoPar >= 300){ // verifica se a pilha esta cheia
        printf("Pilha expressoes com () cheia\n");
        return;
    }
    // coloca x no topo e incrementa o contador do topo
    pilhaPar[topoPar] = x;
    topoPar++;
}

// desempilha na pilhaPar
int popPilhaPAR(){
    if(topoPar==0){ // pilha vazia
        printf("Pilha expressoes com () vazia\n");
        return 0; 
    }
    topoPar--;
    return pilhaPar[topoPar];
}

// empilha x na pilhaCOND (desvios condiconais e lacos)
void pushPilhaCOND(int x){
    if(topoCond>=300){ // verifica se a pilha esta cheia
        printf("Pilha de desvios condicionais e lacos cheia\n");
        return;
    }
    // coloca x no topo e incrementa o topo 
    pilhaCond[topoCond] = x; 
    topoCond++;
}

// desempilha da pilha COND
int popPilhaCOND(){
    if(topoCond==0){ // verifica se não esta vazia
        topoCond++;
        printf("Pilha de desvios condicionais e lacos cheia\n");
        return 0;
    }
    topoCond--;
    return pilhaCond[topoCond];
}

%}

// tipos de dados que vao ser associados a um token ou regra
%union{
    char *string; 
    int inteiro;
}

// tokens que carregam um tipo string
%token <string> ID STRING
// tokens tipo inteiro
%token <inteiro> NUM
// tokens que são simbolos e não possuem valor extra
%token PEV FOR WHILE IF ELSE INT PRINT VIRGULA SCAN ERROR ATRIB RETURN
%token  MAISMAIS MENOSMENOS MENORQUE MAIORQUE IGUAL DIFERENTE MAIORIGUALQUE MENORIGUALQUE 
%token MAIS MENOS LPAR RPAR RCOLCHETES LCOLCHETES LCHAVES RCHAVES MULT MOD AND NOT OR DIV BREAK APOSTROF

%type <int_val> expressao term expressao1 logicas
%type <str_val> oper AO increm forVar lacoFor parteIncrem

%%
// regras gramaticais

// regra inicial para ler o programa q é uma sequência de comandos
codigo: atrib codigo
      | lacoFor codigo
      | lacoWhile codigo
      | exibir codigo
      | If codigo
      | pegarEtrada codigo
      | BREAK PEV { printf("jump R0%d\n", pilhaCond[topoCond - 1]); }
      | ERROR
      | ;             
                          
// regra para scan de valores do teclado
pegarEtrada : SCAN LPAR ID RPAR {int endereco_var = capturaEnd($3); printf("read %%r%d\n", endereco_var);} PEV 
        | SCAN LPAR ID LCOLCHETES expressao {int endereco_var = capturaEnd($3); printf("read %%t%d\nstore %%t%d, %%t%d(%d)\n", t, t, $5, endereco_var); t++;} RCOLCHETES RPAR PEV;          
             
// regra para o comando de impressao
exibir : PRINT LPAR print RPAR PEV ;

// regra para os argumentos do comando de impressao
print : expressao {printf("printv %%t%d \n",$1);} VIRGULA print
       | STRING {printf("printf %s\n",$1);} VIRGULA print
       | expressao {printf("printv %%t%d \n",$1);}
       | STRING {printf("printf %s\n",$1);};

// regra para atribuicoes e declaracoes de variaveis
atrib : INT ID ATRIB expressao PEV {printf("mov %%r%d, %%t%d\n", capturaEnd($2), $4);} 
      | INT ID PEV {tabEntrada[idx_entradas] = (entrada){$2, end}; idx_entradas++; end++;}
      | ID ATRIB expressao PEV {printf("mov %%r%d, %%t%d\n", capturaEnd($1), $3);} 
      | INT ID LCOLCHETES NUM {tabEntrada[idx_entradas] = (entrada){$2, end}; idx_entradas++; end += $4;} RCOLCHETES PEV
      | ID LCOLCHETES expressao RCOLCHETES ATRIB expressao {int endereco_var = capturaEnd($1); printf("store %%t%d, %%t%d(%d)\n", t-1, $3, endereco_var);} PEV;    

// regra principal para a estrutura do laco 'for'
lacoFor : FOR LPAR atrib {nmr_jumpFalse = desvio+1; printf("label R0%d\n", desvio); pushPilhaCOND(desvio++); pushPilhaCOND(desvio++);} forVar PEV parteIncrem RPAR LCHAVES codigo RCHAVES {int endereco_var = capturaEnd($5); printf("add %%r%d, %%r%d, 1\n", endereco_var, endereco_var); printf("jump R0%d\nlabel R0%d\n", popPilhaCOND(), popPilhaCOND());}; 

// regra para a condição de parada do 'for'
forVar : ID oper expressao {int endereco_var = capturaEnd($1); printf("%s %%t%d, %%r%d, %%t%d\njf %%t%d, R0%d\n", $2, t, endereco_var, $3, t, nmr_jumpFalse); $$=$1; t++;};

// regra para a parte de incremento do 'for'
parteIncrem : increm ID {$$ = $2;}
            | ID increm {$$ = $1;};        
        
// regra para a estrutura do laco 'while'
lacoWhile : WHILE {nmr_jumpFalse = desvio+1; printf("label R0%d \n", desvio); pushPilhaCOND(desvio++); pushPilhaCOND(desvio++);} LPAR logicas {printf("jf %%t%d, R0%d\n", t, nmr_jumpFalse); t++;} RPAR LCHAVES codigo RCHAVES {printf("jump R0%d\nlabel R0%d\n", popPilhaCOND(), popPilhaCOND());};

// regra para a estrutura condicional 'if'
If : IF {nmr_jumpFalse = desvio; pushPilhaCOND(desvio++); pushPilhaCOND(desvio++);} LPAR logicas {printf("jf %%t%d, R0%d\n", t, nmr_jumpFalse); t++;} RPAR LCHAVES codigo RCHAVES else ;

// regra para 'else'.
else : ELSE {dest_else[idx_else++] = popPilhaCOND(); printf("jump R0%d\n", dest_else[idx_else-1]); printf("label R0%d\n", popPilhaCOND());} LCHAVES codigo RCHAVES {printf("label R0%d\n", dest_else[--idx_else]); } 
     | { int x = popPilhaCOND(); printf("jump R0%d\n", x); printf("label R0%d\nlabel R0%d\n", popPilhaCOND(), x);};

// regra para expressoes logicas de comparacao
logicas : expressao oper expressao AO {printf("%s %%t%d, %%t%d, %%t%d\n", $2, t, $1, $3); tabLogicas[idx_logicas].r2 = t; t++; tabLogicas[idx_logicas].operador = $4; cplx=0;} logicas  
        | expressao oper expressao {printf("%s %%t%d, %%t%d, %%t%d\n", $2, t, $1, $3); pushPilhaPAR(t); if(cplx != -1){ popLogicas(t); t++; }; cplx=0;} 
        | LPAR logicas RPAR AO logicas {printf("%s %%t%d, %%t%d, %%t%d \n", $4, t, popPilhaPAR(), popPilhaPAR()); $$ = $2;}
        | LPAR logicas RPAR {$$ = $2;};
           
// regra que converte tokens de operadores de comparacaoo em strings
oper : MAIORQUE {$$ = "greater";}       
     | MENORQUE {$$ = "less";}
     | IGUAL {$$ = "equal";}
     | DIFERENTE {$$ = "diff";}
     | MENORIGUALQUE {$$ = "lesseq";}
     | MAIORIGUALQUE {$$ = "greatereq";}; 

// regra que converte tokens de operadores logicos em strings
AO : AND {$$ = "and";}
   | OR {$$ = "or";};
          
// regra para os operadores de incremento/decremento
increm : MAISMAIS {$$ = "add";}
       | MENOSMENOS {$$ = "sub";};

// regras para expressoes aritmeticas, definindo a precedencia
expressao : expressao MAIS expressao1 {printf("add %%t%d, %%t%d, %%t%d\n", t, $1, $3); $$ = t++;}
          | expressao MENOS expressao1 {printf("sub %%t%d, %%t%d, %%t%d\n", t, $1, $3); $$ = t++;} 
          | expressao1 {$$ = $1;};
      
expressao1 : expressao1 DIV term {printf("div %%t%d, %%t%d, %%t%d\n", t, $1, $3); $$ = t++;}
           | expressao1 MULT term {printf("mult %%t%d, %%t%d, %%t%d\n", t, $1, $3); $$ = t++;}
           | expressao1 MOD term  {printf("mod %%t%d, %%t%d, %%t%d\n", t, $1, $3); $$ = t++;}
           | term {$$ = $1;};
     
// regra para os termos de uma expressao (variaveis, numeros...)
term : ID { int endereco_var = capturaEnd($1); printf("mov %%t%d, %%r%d\n", t, endereco_var); $$ = t++;}
     | NUM  { printf("mov %%t%d, %d\n", t, $1); $$ = t++; }
     | LPAR expressao RPAR { $$ = $2; }
     | MENOS ID  { int endereco_var = capturaEnd($2); printf("mov %%t%d, %%r%d\n", t, endereco_var); $$ = t++;} 
     | MENOS NUM { printf("mov %%t%d, %d\n", t, -$2); $$ = t++; }
     | ID LCOLCHETES expressao RCOLCHETES { int endereco_var = capturaEnd($1); printf("load %%t%d, %%t%d(%d) \n", t, $3, endereco_var); $$ = t++;} ;

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
void yyerror(char *s) { 
    fprintf(stderr,"ERRO SINTATICO : %s\n", s) ;
}