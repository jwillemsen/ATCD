/* A Bison parser, made by GNU Bison 3.5.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_ACE_YY_CC_COMMAND_TAB_H_INCLUDED
# define YY_ACE_YY_CC_COMMAND_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ace_yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_START_CMD = 258,
    T_CREATE_CMD = 259,
    T_LOCK_CMD = 260,
    T_UNLOCK_CMD = 261,
    T_TRYLOCK_CMD = 262,
    T_CHANGEMODE_CMD = 263,
    T_SLEEP_CMD = 264,
    T_WAIT_CMD = 265,
    T_REPEAT_CMD = 266,
    T_EXCEP_CMD = 267,
    T_PRINT_CMD = 268,
    T_LOOKUP_CMD = 269,
    T_TERM = 270,
    T_READ = 271,
    T_IREAD = 272,
    T_UPGRADE = 273,
    T_WRITE = 274,
    T_IWRITE = 275,
    T_IDENT = 276,
    Q_STRING = 277,
    T_NUM = 278
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 31 "CC_command.y"

  CC_Command *command;
  CosConcurrencyControl::lock_mode lock_mode;
  char *id;
  int num;

#line 88 "CC_command.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE ace_yylval;

int ace_yyparse (void);

#endif /* !YY_ACE_YY_CC_COMMAND_TAB_H_INCLUDED  */
