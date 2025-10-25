/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "sintatico.y"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int yylex(void); 
void yyerror(char *s); 
int capturaEnd(char *nome);

typedef enum {
    ID_VARIAVEL_SIMPLES,
    ID_VETOR
} TipoID;

typedef struct{ // guarda as declaracoes de variavel
    char *nome; // nome da variavel 
    int endereco;// endereco na memoria simulada
    TipoID tipo; // Identificar se é var ou vetor
}entrada;

typedef struct{ // auxilia em expressoes logicas complexas (AND/OR)
    char *operador; // AND/OR
    // registradores temporarios (r3 = r3(resultado de outra operacao)) OR r2(result. de outra operacao))
    int r1;
    int r2;
    int r3;
}logicas;

// declaracao de variaveis necessarias para o cpl durante a analise

logicas tabLogicas[1000]; // armazena as expressoes logicas do codigo
entrada tabEntrada[25000]; // armazena todas as variaveis declaradas no codigo

int idx_entradas = 0; // contador para o nmr de entradas (variaveis) atualmente na tabela
int end = 0; // contador para simular o prox endereco de memoria livre a ser alocado
int idx_logicas = 0; // indice para a prox pos livre no vetor de 'logicas'

int cplx = -1; // flag para indicar se uma logica complexa (AND/OR) esta sendo analisada

int pilhaCond[300]; // pilha para gerenciar desvio (lacos e condicionais)
int topoCond = 0;

int pilhaLoop[300]; // pilha para gerenciar BREAK (só loops)
int topoLoop = 0;

int pilhaPar[300]; // pilha para ordem de avaliacao de expressoes com "()"
int topoPar = 0;

int nmr_jumpFalse = 0; // guarda o nmr do rotulo para onde um comando "salto se falso" deve ir
int t = 0; // contador para os registradores temporarios
int desvio = 0; // contador para os rotulo de desvio

int dest_else[150]; // vetor para guardar o destino de um else (para gerar salto correto)
int idx_else = 0; //proxima posicao livre

// FUNÇÔES AUXILIARES

// FUNÇÃO SEMÂNTICA : Verifica declaracao
void verificaDeclaracao(char *nome) {
    if (capturaEnd(nome) != -1) {
        fprintf(stderr, "ERRO SEMANTICO: Variavel '%s' ja foi declarada\n", nome);
        exit(1); // Interrompe a compilação
    }
}


// FUNÇÃO SEMÂNTICA : Verifica se é uma VARIAVEL SIMPLES declarada.
int buscaEValidaVariavelSimples(char *nome) {
    for(int i=0; i<idx_entradas; i++){
        if(strcmp(tabEntrada[i].nome, nome) == 0){ // Achou o nome
            // VERIFICAÇÃO DE TIPO:
            if (tabEntrada[i].tipo != ID_VARIAVEL_SIMPLES) {
                fprintf(stderr, "ERRO SEMANTICO: '%s' é um vetor e foi usado como variavel simples.\n", nome);
                exit(1);
            }
            return tabEntrada[i].endereco; // Sucesso!
        }
    }
    // Se saiu do loop, não achou
    fprintf(stderr, "ERRO SEMANTICO: Variavel '%s' nao foi declarada\n", nome);
    exit(1); 
    return -1; // (só para o compilador não reclamar)
}

// FUNÇÃO SEMÂNTICA : Verifica se é um VETOR declarado.
int buscaEValidaVetor(char *nome) {
    for(int i=0; i<idx_entradas; i++){
        if(strcmp(tabEntrada[i].nome, nome) == 0){ // Achou o nome
            // VERIFICAÇÃO DE TIPO:
            if (tabEntrada[i].tipo != ID_VETOR) {
                fprintf(stderr, "ERRO SEMANTICO: '%s' é uma variavel simples e foi usada como vetor.\n", nome);
                exit(1);
            }
            return tabEntrada[i].endereco; // Sucesso!
        }
    }
    // Se saiu do loop, não achou
    fprintf(stderr, "ERRO SEMANTICO: Vetor '%s' nao foi declarado\n", nome);
    exit(1); 
    return -1;
}

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
    if(topoPar <= 0){ // pilha vazia
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
    if(topoCond<=0){ // verifica se não esta vazia
        printf("Pilha de desvios condicionais e lacos vazia\n");
        return 0;
    }
    topoCond--;
    return pilhaCond[topoCond];
}

// empilha x na pilhaLoop (para BREAK)
void pushPilhaLoop(int x){
    if(topoLoop>=300){
        printf("Pilha de loops cheia\n");
        return;
    }
    pilhaLoop[topoLoop] = x;
    topoLoop++;
}

// desempilha da pilha Loop
int popPilhaLoop(){
    if(topoLoop<=0){
        printf("Pilha de loops vazia\n");
        return 0;
    }
    topoLoop--;
    return pilhaLoop[topoLoop];
}


#line 252 "sintatico.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "sintatico.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_STRING = 4,                     /* STRING  */
  YYSYMBOL_NUM = 5,                        /* NUM  */
  YYSYMBOL_PEV = 6,                        /* PEV  */
  YYSYMBOL_FOR = 7,                        /* FOR  */
  YYSYMBOL_WHILE = 8,                      /* WHILE  */
  YYSYMBOL_IF = 9,                         /* IF  */
  YYSYMBOL_ELSE = 10,                      /* ELSE  */
  YYSYMBOL_INT = 11,                       /* INT  */
  YYSYMBOL_PRINT = 12,                     /* PRINT  */
  YYSYMBOL_VIRGULA = 13,                   /* VIRGULA  */
  YYSYMBOL_SCAN = 14,                      /* SCAN  */
  YYSYMBOL_ERROR = 15,                     /* ERROR  */
  YYSYMBOL_ATRIB = 16,                     /* ATRIB  */
  YYSYMBOL_RETURN = 17,                    /* RETURN  */
  YYSYMBOL_MAISMAIS = 18,                  /* MAISMAIS  */
  YYSYMBOL_MENOSMENOS = 19,                /* MENOSMENOS  */
  YYSYMBOL_MENORQUE = 20,                  /* MENORQUE  */
  YYSYMBOL_MAIORQUE = 21,                  /* MAIORQUE  */
  YYSYMBOL_IGUAL = 22,                     /* IGUAL  */
  YYSYMBOL_DIFERENTE = 23,                 /* DIFERENTE  */
  YYSYMBOL_MAIORIGUALQUE = 24,             /* MAIORIGUALQUE  */
  YYSYMBOL_MENORIGUALQUE = 25,             /* MENORIGUALQUE  */
  YYSYMBOL_MAIS = 26,                      /* MAIS  */
  YYSYMBOL_MENOS = 27,                     /* MENOS  */
  YYSYMBOL_LPAR = 28,                      /* LPAR  */
  YYSYMBOL_RPAR = 29,                      /* RPAR  */
  YYSYMBOL_RCOLCHETES = 30,                /* RCOLCHETES  */
  YYSYMBOL_LCOLCHETES = 31,                /* LCOLCHETES  */
  YYSYMBOL_LCHAVES = 32,                   /* LCHAVES  */
  YYSYMBOL_RCHAVES = 33,                   /* RCHAVES  */
  YYSYMBOL_MULT = 34,                      /* MULT  */
  YYSYMBOL_MOD = 35,                       /* MOD  */
  YYSYMBOL_AND = 36,                       /* AND  */
  YYSYMBOL_NOT = 37,                       /* NOT  */
  YYSYMBOL_OR = 38,                        /* OR  */
  YYSYMBOL_DIV = 39,                       /* DIV  */
  YYSYMBOL_BREAK = 40,                     /* BREAK  */
  YYSYMBOL_APOSTROF = 41,                  /* APOSTROF  */
  YYSYMBOL_YYACCEPT = 42,                  /* $accept  */
  YYSYMBOL_codigo = 43,                    /* codigo  */
  YYSYMBOL_pegarEntrada = 44,              /* pegarEntrada  */
  YYSYMBOL_45_1 = 45,                      /* $@1  */
  YYSYMBOL_46_2 = 46,                      /* $@2  */
  YYSYMBOL_exibir = 47,                    /* exibir  */
  YYSYMBOL_print = 48,                     /* print  */
  YYSYMBOL_49_3 = 49,                      /* $@3  */
  YYSYMBOL_50_4 = 50,                      /* $@4  */
  YYSYMBOL_atrib = 51,                     /* atrib  */
  YYSYMBOL_52_5 = 52,                      /* $@5  */
  YYSYMBOL_lacoFor = 53,                   /* lacoFor  */
  YYSYMBOL_54_6 = 54,                      /* $@6  */
  YYSYMBOL_forVar = 55,                    /* forVar  */
  YYSYMBOL_parteIncrem = 56,               /* parteIncrem  */
  YYSYMBOL_lacoWhile = 57,                 /* lacoWhile  */
  YYSYMBOL_58_7 = 58,                      /* $@7  */
  YYSYMBOL_59_8 = 59,                      /* $@8  */
  YYSYMBOL_If = 60,                        /* If  */
  YYSYMBOL_61_9 = 61,                      /* $@9  */
  YYSYMBOL_62_10 = 62,                     /* $@10  */
  YYSYMBOL_else = 63,                      /* else  */
  YYSYMBOL_64_11 = 64,                     /* $@11  */
  YYSYMBOL_logicas = 65,                   /* logicas  */
  YYSYMBOL_66_12 = 66,                     /* $@12  */
  YYSYMBOL_oper = 67,                      /* oper  */
  YYSYMBOL_AO = 68,                        /* AO  */
  YYSYMBOL_increm = 69,                    /* increm  */
  YYSYMBOL_expressao = 70,                 /* expressao  */
  YYSYMBOL_expressao1 = 71,                /* expressao1  */
  YYSYMBOL_term = 72                       /* term  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  26
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   172

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  42
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  31
/* YYNRULES -- Number of rules.  */
#define YYNRULES  69
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  151

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   296


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   206,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   217,   217,   218,   218,   221,   224,   224,   225,   225,
     226,   227,   238,   251,   261,   271,   270,   281,   292,   292,
     295,   298,   299,   302,   302,   302,   305,   305,   305,   308,
     308,   309,   312,   312,   313,   314,   315,   318,   319,   320,
     321,   322,   323,   326,   327,   330,   331,   334,   335,   336,
     338,   339,   340,   341,   344,   345,   346,   347,   348,   349
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "STRING", "NUM",
  "PEV", "FOR", "WHILE", "IF", "ELSE", "INT", "PRINT", "VIRGULA", "SCAN",
  "ERROR", "ATRIB", "RETURN", "MAISMAIS", "MENOSMENOS", "MENORQUE",
  "MAIORQUE", "IGUAL", "DIFERENTE", "MAIORIGUALQUE", "MENORIGUALQUE",
  "MAIS", "MENOS", "LPAR", "RPAR", "RCOLCHETES", "LCOLCHETES", "LCHAVES",
  "RCHAVES", "MULT", "MOD", "AND", "NOT", "OR", "DIV", "BREAK", "APOSTROF",
  "$accept", "codigo", "pegarEntrada", "$@1", "$@2", "exibir", "print",
  "$@3", "$@4", "atrib", "$@5", "lacoFor", "$@6", "forVar", "parteIncrem",
  "lacoWhile", "$@7", "$@8", "If", "$@9", "$@10", "else", "$@11",
  "logicas", "$@12", "oper", "AO", "increm", "expressao", "expressao1",
  "term", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-38)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-22)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       4,    27,   -18,   -38,   -38,    21,     1,     6,   -38,    76,
      51,     4,     4,     4,     4,     4,     4,    25,    25,    46,
      89,    94,    19,    18,   122,   -38,   -38,   -38,   -38,   -38,
     -38,   -38,   -38,    32,   -38,   106,    25,    11,    57,   -38,
      59,   -38,    28,    28,   -38,    25,   124,   107,   111,    81,
      92,    25,   -38,   -38,    86,   -38,    25,    25,    25,    25,
      25,   119,   138,    28,   -38,    79,   -38,    14,   -38,   129,
     137,   131,   -38,    25,    67,   -38,    57,    57,   -38,   -38,
     -38,    25,    45,   139,   117,    54,   118,   -38,   -38,   -38,
     -38,   -38,   -38,    25,   120,   -38,   121,    18,   -38,    18,
     142,   105,   -38,    33,    25,    29,    88,   123,    35,   125,
     144,   -38,   -38,   -38,   126,   -38,   105,   115,   -38,   -38,
     130,   149,   -38,   -38,    28,     4,   -38,     4,   -38,   132,
     -38,   128,   -38,   -38,   133,    28,   134,   147,     4,   -38,
     -38,   148,   -38,   135,   -38,   -38,   -38,   140,     4,   136,
     -38
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
      10,     0,     0,    33,    36,     0,     0,     0,     9,     0,
       0,    10,    10,    10,    10,    10,    10,     0,     0,     0,
       0,     0,     0,     0,     0,     8,     1,     7,     5,     2,
       3,     4,     6,    64,    65,     0,     0,     0,    59,    63,
       0,    28,     0,     0,    23,     0,     0,    18,     0,    16,
       0,     0,    67,    68,     0,    24,     0,     0,     0,     0,
       0,     0,     0,     0,    34,     0,    37,     0,    25,     0,
       0,     0,    11,     0,     0,    66,    57,    58,    61,    62,
      60,     0,     0,     0,     0,     0,     0,    48,    47,    49,
      50,    52,    51,     0,     0,    22,     0,     0,    15,     0,
       0,    13,    69,     0,     0,     0,    46,     0,    44,     0,
       0,    19,    17,    12,     0,    27,    30,     0,    55,    56,
       0,     0,    53,    54,     0,    10,    42,    10,    26,     0,
      32,     0,    31,    45,     0,     0,     0,     0,    10,    35,
      43,    41,    14,     0,    39,    38,    29,     0,    10,     0,
      40
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -38,   -11,   -38,   -38,   -38,   -38,    31,   -38,   -38,   143,
     -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
     -38,   -38,   -38,   -37,   -38,    72,    55,    47,    -9,    82,
      60
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    10,    11,   100,   114,    12,    48,    71,    69,    13,
      96,    14,    62,    83,   120,    15,    20,    86,    16,    21,
      94,   145,   147,    64,   135,    93,   124,   121,    65,    38,
      39
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      27,    28,    29,    30,    31,    32,    66,     1,    37,    40,
      19,     2,     3,     4,    49,     5,     6,    55,     7,     8,
      95,    33,    47,    34,    22,    44,    84,    54,    33,    23,
      34,    33,   117,    34,    24,    45,    67,    56,    57,   115,
      56,    57,    74,    17,     9,    35,    36,   118,   119,     1,
      46,    26,    35,    36,    85,    35,    63,     5,    18,    56,
      57,    56,    57,    51,   101,    87,    88,    89,    90,    91,
      92,   122,   103,   123,    87,    88,    89,    90,    91,    92,
      56,    57,    25,    75,   108,    56,    57,   133,    49,    61,
      49,    58,    59,    56,    57,   116,    60,   102,   140,    87,
      88,    89,    90,    91,    92,    56,    57,    56,    57,    52,
     -20,    53,    56,    57,   134,    75,   136,    42,    78,    79,
      80,    72,    43,    73,   122,    50,   123,   143,   111,    68,
     112,    56,    57,   118,   119,    81,   -21,   149,    76,    77,
      70,    82,    97,    98,    99,   105,   106,   107,   113,   109,
     128,   110,   132,   142,   104,   125,   129,   127,   144,   131,
     138,   137,    41,   126,   130,     0,   139,   141,   146,   150,
       0,     0,   148
};

static const yytype_int16 yycheck[] =
{
      11,    12,    13,    14,    15,    16,    43,     3,    17,    18,
      28,     7,     8,     9,    23,    11,    12,     6,    14,    15,
       6,     3,     4,     5,     3,     6,    63,    36,     3,    28,
       5,     3,     3,     5,    28,    16,    45,    26,    27,     6,
      26,    27,    51,    16,    40,    27,    28,    18,    19,     3,
      31,     0,    27,    28,    63,    27,    28,    11,    31,    26,
      27,    26,    27,    31,    73,    20,    21,    22,    23,    24,
      25,    36,    81,    38,    20,    21,    22,    23,    24,    25,
      26,    27,     6,    29,    93,    26,    27,   124,    97,    30,
      99,    34,    35,    26,    27,   104,    39,    30,   135,    20,
      21,    22,    23,    24,    25,    26,    27,    26,    27,     3,
      29,     5,    26,    27,   125,    29,   127,    28,    58,    59,
      60,    29,    28,    31,    36,     3,    38,   138,    97,     5,
      99,    26,    27,    18,    19,    16,    29,   148,    56,    57,
      29,     3,    13,     6,    13,     6,    29,    29,     6,    29,
       6,    30,     3,     6,    82,    32,    30,    32,    10,    29,
      32,    29,    19,   108,   117,    -1,    33,    33,    33,    33,
      -1,    -1,    32
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     7,     8,     9,    11,    12,    14,    15,    40,
      43,    44,    47,    51,    53,    57,    60,    16,    31,    28,
      58,    61,     3,    28,    28,     6,     0,    43,    43,    43,
      43,    43,    43,     3,     5,    27,    28,    70,    71,    72,
      70,    51,    28,    28,     6,    16,    31,     4,    48,    70,
       3,    31,     3,     5,    70,     6,    26,    27,    34,    35,
      39,    30,    54,    28,    65,    70,    65,    70,     5,    50,
      29,    49,    29,    31,    70,    29,    71,    71,    72,    72,
      72,    16,     3,    55,    65,    70,    59,    20,    21,    22,
      23,    24,    25,    67,    62,     6,    52,    13,     6,    13,
      45,    70,    30,    70,    67,     6,    29,    29,    70,    29,
      30,    48,    48,     6,    46,     6,    70,     3,    18,    19,
      56,    69,    36,    38,    68,    32,    68,    32,     6,    30,
      69,    29,     3,    65,    43,    66,    43,    29,    32,    33,
      65,    33,     6,    43,    10,    63,    33,    64,    32,    43,
      33
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    42,    43,    43,    43,    43,    43,    43,    43,    43,
      43,    45,    44,    46,    44,    47,    49,    48,    50,    48,
      48,    48,    51,    51,    51,    52,    51,    51,    54,    53,
      55,    56,    56,    58,    59,    57,    61,    62,    60,    64,
      63,    63,    66,    65,    65,    65,    65,    67,    67,    67,
      67,    67,    67,    68,    68,    69,    69,    70,    70,    70,
      71,    71,    71,    71,    72,    72,    72,    72,    72,    72
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     1,
       0,     0,     6,     0,     9,     5,     0,     4,     0,     4,
       1,     1,     5,     3,     4,     0,     7,     7,     0,    11,
       3,     2,     2,     0,     0,     9,     0,     0,    10,     0,
       5,     0,     0,     6,     3,     5,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     1,
       3,     3,     3,     1,     1,     1,     3,     2,     2,     4
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 8: /* codigo: BREAK PEV  */
#line 212 "sintatico.y"
                  { printf("jump R0%d\n", pilhaLoop[topoLoop - 1]); }
#line 1683 "sintatico.tab.c"
    break;

  case 11: /* $@1: %empty  */
#line 217 "sintatico.y"
                                 {int endereco_var = buscaEValidaVariavelSimples((yyvsp[-1].string)); printf("read %%r%d\n", endereco_var);}
#line 1689 "sintatico.tab.c"
    break;

  case 13: /* $@2: %empty  */
#line 218 "sintatico.y"
                                            {int endereco_var = buscaEValidaVetor((yyvsp[-2].string)); printf("read %%t%d\nstore %%t%d, %%t%d(%d)\n", t, t, (yyvsp[0].inteiro), endereco_var); t++;}
#line 1695 "sintatico.tab.c"
    break;

  case 16: /* $@3: %empty  */
#line 224 "sintatico.y"
                  {printf("printv %%t%d \n",(yyvsp[0].inteiro));}
#line 1701 "sintatico.tab.c"
    break;

  case 18: /* $@4: %empty  */
#line 225 "sintatico.y"
                {printf("printf %s\n",(yyvsp[0].string));}
#line 1707 "sintatico.tab.c"
    break;

  case 20: /* print: expressao  */
#line 226 "sintatico.y"
                   {printf("printv %%t%d \n",(yyvsp[0].inteiro));}
#line 1713 "sintatico.tab.c"
    break;

  case 21: /* print: STRING  */
#line 227 "sintatico.y"
                {printf("printf %s\n",(yyvsp[0].string));}
#line 1719 "sintatico.tab.c"
    break;

  case 22: /* atrib: INT ID ATRIB expressao PEV  */
#line 239 "sintatico.y"
          {
              // Verificação Semântica (Declaração Duplicada)
              verificaDeclaracao((yyvsp[-3].string)); 

              // Ação na Tabela (Adiciona com o tipo simples)
              tabEntrada[idx_entradas] = (entrada){(yyvsp[-3].string), end, ID_VARIAVEL_SIMPLES}; 
              
              // Geração de Código
              printf("mov %%r%d, %%t%d\n", end, (yyvsp[-1].inteiro)); 
              idx_entradas++; 
              end++;
          }
#line 1736 "sintatico.tab.c"
    break;

  case 23: /* atrib: INT ID PEV  */
#line 252 "sintatico.y"
          {
              // Verificação Semântica (Declaração Duplicada)
              verificaDeclaracao((yyvsp[-1].string)); 

              // Ação na Tabela (Adiciona com o tipo simples)
              tabEntrada[idx_entradas] = (entrada){(yyvsp[-1].string), end, ID_VARIAVEL_SIMPLES}; 
              idx_entradas++; 
              end++;
          }
#line 1750 "sintatico.tab.c"
    break;

  case 24: /* atrib: ID ATRIB expressao PEV  */
#line 262 "sintatico.y"
          {
              // Verificação Semântica (Uso de Variável Simples)
              // Verifica se $1 existe E é uma variável simples.
              int endereco_var = buscaEValidaVariavelSimples((yyvsp[-3].string));

              // Geração de Código
              printf("mov %%r%d, %%t%d\n", endereco_var, (yyvsp[-1].inteiro));
          }
#line 1763 "sintatico.tab.c"
    break;

  case 25: /* $@5: %empty  */
#line 271 "sintatico.y"
          {
              // Verificação Semântica (Declaração Duplicada)
              verificaDeclaracao((yyvsp[-2].string));

              // Ação na Tabela (Adiciona com o tipo vetor)
              tabEntrada[idx_entradas] = (entrada){(yyvsp[-2].string), end, ID_VETOR}; 
              idx_entradas++; 
              end += (yyvsp[0].inteiro); // Aloca o espaço do vetor
          }
#line 1777 "sintatico.tab.c"
    break;

  case 27: /* atrib: ID LCOLCHETES expressao RCOLCHETES ATRIB expressao PEV  */
#line 282 "sintatico.y"
          {
              // Verifica se $1 existe E é um vetor.
              int endereco_var = buscaEValidaVetor((yyvsp[-6].string));
              
              // $3 é o índice (i), $6 é o valor (10).
              printf("store %%t%d, %%t%d(%d)\n", (yyvsp[-1].inteiro), (yyvsp[-4].inteiro), endereco_var);
          }
#line 1789 "sintatico.tab.c"
    break;

  case 28: /* $@6: %empty  */
#line 292 "sintatico.y"
                         {nmr_jumpFalse = desvio+1; printf("label R0%d\n", desvio); pushPilhaCOND(desvio++); pushPilhaCOND(desvio); pushPilhaLoop(desvio++);}
#line 1795 "sintatico.tab.c"
    break;

  case 29: /* lacoFor: FOR LPAR atrib $@6 forVar PEV parteIncrem RPAR LCHAVES codigo RCHAVES  */
#line 292 "sintatico.y"
                                                                                                                                                                                                                 {int endereco_var = buscaEValidaVariavelSimples((yyvsp[-6].string)); printf("add %%r%d, %%r%d, 1\n", endereco_var, endereco_var); popPilhaLoop(); printf("jump R0%d\nlabel R0%d\n", popPilhaCOND(), popPilhaCOND());}
#line 1801 "sintatico.tab.c"
    break;

  case 30: /* forVar: ID oper expressao  */
#line 295 "sintatico.y"
                           {int endereco_var = buscaEValidaVariavelSimples((yyvsp[-2].string)); printf("%s %%t%d, %%r%d, %%t%d\njf %%t%d, R0%d\n", (yyvsp[-1].string), t, endereco_var, (yyvsp[0].inteiro), t, nmr_jumpFalse); (yyval.string)=(yyvsp[-2].string); t++;}
#line 1807 "sintatico.tab.c"
    break;

  case 31: /* parteIncrem: increm ID  */
#line 298 "sintatico.y"
                        {(yyval.string) = (yyvsp[0].string);}
#line 1813 "sintatico.tab.c"
    break;

  case 32: /* parteIncrem: ID increm  */
#line 299 "sintatico.y"
                        {(yyval.string) = (yyvsp[-1].string);}
#line 1819 "sintatico.tab.c"
    break;

  case 33: /* $@7: %empty  */
#line 302 "sintatico.y"
                  {nmr_jumpFalse = desvio+1; printf("label R0%d \n", desvio); pushPilhaCOND(desvio++); pushPilhaCOND(desvio); pushPilhaLoop(desvio++);}
#line 1825 "sintatico.tab.c"
    break;

  case 34: /* $@8: %empty  */
#line 302 "sintatico.y"
                                                                                                                                                                     {printf("jf %%t%d, R0%d\n", t, nmr_jumpFalse); t++;}
#line 1831 "sintatico.tab.c"
    break;

  case 35: /* lacoWhile: WHILE $@7 LPAR logicas $@8 RPAR LCHAVES codigo RCHAVES  */
#line 302 "sintatico.y"
                                                                                                                                                                                                                                                      {popPilhaLoop(); printf("jump R0%d\nlabel R0%d\n", popPilhaCOND(), popPilhaCOND());}
#line 1837 "sintatico.tab.c"
    break;

  case 36: /* $@9: %empty  */
#line 305 "sintatico.y"
        {nmr_jumpFalse = desvio; pushPilhaCOND(desvio++); pushPilhaCOND(desvio++);}
#line 1843 "sintatico.tab.c"
    break;

  case 37: /* $@10: %empty  */
#line 305 "sintatico.y"
                                                                                                 {printf("jf %%t%d, R0%d\n", t, nmr_jumpFalse); t++;}
#line 1849 "sintatico.tab.c"
    break;

  case 39: /* $@11: %empty  */
#line 308 "sintatico.y"
            {dest_else[idx_else++] = popPilhaCOND(); printf("jump R0%d\n", dest_else[idx_else-1]); printf("label R0%d\n", popPilhaCOND());}
#line 1855 "sintatico.tab.c"
    break;

  case 40: /* else: ELSE $@11 LCHAVES codigo RCHAVES  */
#line 308 "sintatico.y"
                                                                                                                                                                   {printf("label R0%d\n", dest_else[--idx_else]); }
#line 1861 "sintatico.tab.c"
    break;

  case 41: /* else: %empty  */
#line 309 "sintatico.y"
       { int x = popPilhaCOND(); printf("jump R0%d\n", x); printf("label R0%d\nlabel R0%d\n", popPilhaCOND(), x);}
#line 1867 "sintatico.tab.c"
    break;

  case 42: /* $@12: %empty  */
#line 312 "sintatico.y"
                                      {printf("%s %%t%d, %%t%d, %%t%d\n", (yyvsp[-2].string), t, (yyvsp[-3].inteiro), (yyvsp[-1].inteiro)); tabLogicas[idx_logicas].r2 = t; t++; tabLogicas[idx_logicas].operador = (yyvsp[0].string); idx_logicas++; cplx=0;}
#line 1873 "sintatico.tab.c"
    break;

  case 44: /* logicas: expressao oper expressao  */
#line 313 "sintatico.y"
                                   {printf("%s %%t%d, %%t%d, %%t%d\n", (yyvsp[-1].string), t, (yyvsp[-2].inteiro), (yyvsp[0].inteiro)); pushPilhaPAR(t); if(cplx == 0){ popLogicas(t); idx_logicas--; t++; }; cplx=-1;}
#line 1879 "sintatico.tab.c"
    break;

  case 45: /* logicas: LPAR logicas RPAR AO logicas  */
#line 314 "sintatico.y"
                                       {printf("%s %%t%d, %%t%d, %%t%d \n", (yyvsp[-1].string), t, popPilhaPAR(), popPilhaPAR()); (yyval.inteiro) = (yyvsp[-3].inteiro);}
#line 1885 "sintatico.tab.c"
    break;

  case 46: /* logicas: LPAR logicas RPAR  */
#line 315 "sintatico.y"
                            {(yyval.inteiro) = (yyvsp[-1].inteiro);}
#line 1891 "sintatico.tab.c"
    break;

  case 47: /* oper: MAIORQUE  */
#line 318 "sintatico.y"
                {(yyval.string) = "greater";}
#line 1897 "sintatico.tab.c"
    break;

  case 48: /* oper: MENORQUE  */
#line 319 "sintatico.y"
                {(yyval.string) = "less";}
#line 1903 "sintatico.tab.c"
    break;

  case 49: /* oper: IGUAL  */
#line 320 "sintatico.y"
             {(yyval.string) = "equal";}
#line 1909 "sintatico.tab.c"
    break;

  case 50: /* oper: DIFERENTE  */
#line 321 "sintatico.y"
                 {(yyval.string) = "diff";}
#line 1915 "sintatico.tab.c"
    break;

  case 51: /* oper: MENORIGUALQUE  */
#line 322 "sintatico.y"
                     {(yyval.string) = "lesseq";}
#line 1921 "sintatico.tab.c"
    break;

  case 52: /* oper: MAIORIGUALQUE  */
#line 323 "sintatico.y"
                     {(yyval.string) = "greatereq";}
#line 1927 "sintatico.tab.c"
    break;

  case 53: /* AO: AND  */
#line 326 "sintatico.y"
         {(yyval.string) = "and";}
#line 1933 "sintatico.tab.c"
    break;

  case 54: /* AO: OR  */
#line 327 "sintatico.y"
        {(yyval.string) = "or";}
#line 1939 "sintatico.tab.c"
    break;

  case 55: /* increm: MAISMAIS  */
#line 330 "sintatico.y"
                  {(yyval.string) = "add";}
#line 1945 "sintatico.tab.c"
    break;

  case 56: /* increm: MENOSMENOS  */
#line 331 "sintatico.y"
                    {(yyval.string) = "sub";}
#line 1951 "sintatico.tab.c"
    break;

  case 57: /* expressao: expressao MAIS expressao1  */
#line 334 "sintatico.y"
                                      {printf("add %%t%d, %%t%d, %%t%d\n", t, (yyvsp[-2].inteiro), (yyvsp[0].inteiro)); (yyval.inteiro) = t++;}
#line 1957 "sintatico.tab.c"
    break;

  case 58: /* expressao: expressao MENOS expressao1  */
#line 335 "sintatico.y"
                                       {printf("sub %%t%d, %%t%d, %%t%d\n", t, (yyvsp[-2].inteiro), (yyvsp[0].inteiro)); (yyval.inteiro) = t++;}
#line 1963 "sintatico.tab.c"
    break;

  case 59: /* expressao: expressao1  */
#line 336 "sintatico.y"
                       {(yyval.inteiro) = (yyvsp[0].inteiro);}
#line 1969 "sintatico.tab.c"
    break;

  case 60: /* expressao1: expressao1 DIV term  */
#line 338 "sintatico.y"
                                 {printf("div %%t%d, %%t%d, %%t%d\n", t, (yyvsp[-2].inteiro), (yyvsp[0].inteiro)); (yyval.inteiro) = t++;}
#line 1975 "sintatico.tab.c"
    break;

  case 61: /* expressao1: expressao1 MULT term  */
#line 339 "sintatico.y"
                                  {printf("mult %%t%d, %%t%d, %%t%d\n", t, (yyvsp[-2].inteiro), (yyvsp[0].inteiro)); (yyval.inteiro) = t++;}
#line 1981 "sintatico.tab.c"
    break;

  case 62: /* expressao1: expressao1 MOD term  */
#line 340 "sintatico.y"
                                  {printf("mod %%t%d, %%t%d, %%t%d\n", t, (yyvsp[-2].inteiro), (yyvsp[0].inteiro)); (yyval.inteiro) = t++;}
#line 1987 "sintatico.tab.c"
    break;

  case 63: /* expressao1: term  */
#line 341 "sintatico.y"
                  {(yyval.inteiro) = (yyvsp[0].inteiro);}
#line 1993 "sintatico.tab.c"
    break;

  case 64: /* term: ID  */
#line 344 "sintatico.y"
          { int endereco_var = buscaEValidaVariavelSimples((yyvsp[0].string)); printf("mov %%t%d, %%r%d\n", t, endereco_var); (yyval.inteiro) = t++;}
#line 1999 "sintatico.tab.c"
    break;

  case 65: /* term: NUM  */
#line 345 "sintatico.y"
            { printf("mov %%t%d, %d\n", t, (yyvsp[0].inteiro)); (yyval.inteiro) = t++; }
#line 2005 "sintatico.tab.c"
    break;

  case 66: /* term: LPAR expressao RPAR  */
#line 346 "sintatico.y"
                           { (yyval.inteiro) = (yyvsp[-1].inteiro); }
#line 2011 "sintatico.tab.c"
    break;

  case 67: /* term: MENOS ID  */
#line 347 "sintatico.y"
                 { int endereco_var = buscaEValidaVariavelSimples((yyvsp[0].string)); printf("mov %%t%d, %%r%d\n", t, endereco_var); (yyval.inteiro) = t++;}
#line 2017 "sintatico.tab.c"
    break;

  case 68: /* term: MENOS NUM  */
#line 348 "sintatico.y"
                 { printf("mov %%t%d, %d\n", t, -(yyvsp[0].inteiro)); (yyval.inteiro) = t++; }
#line 2023 "sintatico.tab.c"
    break;

  case 69: /* term: ID LCOLCHETES expressao RCOLCHETES  */
#line 349 "sintatico.y"
                                          { int endereco_var = buscaEValidaVetor((yyvsp[-3].string)); printf("load %%t%d, %%t%d(%d) \n", t, (yyvsp[-1].inteiro), endereco_var); (yyval.inteiro) = t++;}
#line 2029 "sintatico.tab.c"
    break;


#line 2033 "sintatico.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 351 "sintatico.y"

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
