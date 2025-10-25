/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SINTATICO_TAB_H_INCLUDED
# define YY_YY_SINTATICO_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 258,                      /* ID  */
    STRING = 259,                  /* STRING  */
    NUM = 260,                     /* NUM  */
    PEV = 261,                     /* PEV  */
    FOR = 262,                     /* FOR  */
    WHILE = 263,                   /* WHILE  */
    IF = 264,                      /* IF  */
    ELSE = 265,                    /* ELSE  */
    INT = 266,                     /* INT  */
    PRINT = 267,                   /* PRINT  */
    VIRGULA = 268,                 /* VIRGULA  */
    SCAN = 269,                    /* SCAN  */
    ERROR = 270,                   /* ERROR  */
    ATRIB = 271,                   /* ATRIB  */
    RETURN = 272,                  /* RETURN  */
    MAISMAIS = 273,                /* MAISMAIS  */
    MENOSMENOS = 274,              /* MENOSMENOS  */
    MENORQUE = 275,                /* MENORQUE  */
    MAIORQUE = 276,                /* MAIORQUE  */
    IGUAL = 277,                   /* IGUAL  */
    DIFERENTE = 278,               /* DIFERENTE  */
    MAIORIGUALQUE = 279,           /* MAIORIGUALQUE  */
    MENORIGUALQUE = 280,           /* MENORIGUALQUE  */
    MAIS = 281,                    /* MAIS  */
    MENOS = 282,                   /* MENOS  */
    LPAR = 283,                    /* LPAR  */
    RPAR = 284,                    /* RPAR  */
    RCOLCHETES = 285,              /* RCOLCHETES  */
    LCOLCHETES = 286,              /* LCOLCHETES  */
    LCHAVES = 287,                 /* LCHAVES  */
    RCHAVES = 288,                 /* RCHAVES  */
    MULT = 289,                    /* MULT  */
    MOD = 290,                     /* MOD  */
    AND = 291,                     /* AND  */
    NOT = 292,                     /* NOT  */
    OR = 293,                      /* OR  */
    DIV = 294,                     /* DIV  */
    BREAK = 295,                   /* BREAK  */
    APOSTROF = 296                 /* APOSTROF  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 183 "sintatico.y"

    char *string; 
    int inteiro;

#line 110 "sintatico.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SINTATICO_TAB_H_INCLUDED  */
