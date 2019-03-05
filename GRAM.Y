%{
  /*
   * PDL-GRAM
   * Grammatica per il linguaggio PDL
   */

#  include "types.h"
#  define YYSTYPE YYStype

int yylex PROTO ((void)) ;

/*
#pragma warn -stv
*/

%}

%left '+' '-'
%left '*' '/'
%token id num string Define End Angle Length Default Defval
%token To Scale Rotate Vector Scalar Dash Fill Versor

%start PROGRAM

%%
/* Programma */
PROGRAM	: PROGRAM DEFINITION
	| PROGRAM VAR_DECL
	| PROGRAM EXPR_STATEMENT
	| PROGRAM NULL_STATEMENT
	|
	;

/*
 * Definizioni di oggetti
 */

/* header */
DEFINITION : Define id
					{new_obj($2);}
	     '(' PARAM_LIST ')'
	     STATEMENT_LIST
	     End id
					{check_obj($9);}
	   ;

/* Function parameters */
PARAM_LIST : PARAM_LIST1
	   |
	   ;
PARAM_LIST1 : PARAM_LIST1 ',' PARAM
	    | PARAM
	    ;

PARAM : PARAM_MODIFIERS id { new_handle($1,$2) ; }
      ;

PARAM_MODIFIERS : Default Defval { $$.t = t_hattr; $$.v.i=h_DEFAULT|h_DEFVAL; }
		| Default        { $$.t = t_hattr; $$.v.i = h_DEFAULT; }
		|         Defval { $$.t = t_hattr; $$.v.i = h_DEFVAL; }
		|                { $$.t = t_hattr; $$.v.i = 0 ; }
		;

/* 
 * Variabili & tipi
 */

/* statement dichiarativo (locale o globale) */

VAR_DECL : VAR_DECL1 ';'

VAR_DECL1 : TYPE id {
			 $$ = $1 ;
			 create_var( $2.v.s, $1.v.i ) ;
		    }
	  | VAR_DECL1 ',' id {
			 	$$ = $1 ;
			 	create_var( $3.v.s, $1.v.i ) ;
		             }
	  ;

TYPE : Vector { $$.t=t_typeval; $$.v.i=t_point;}
     | Scalar { $$.t=t_typeval; $$.v.i=t_scalar;}
     ;

/*
 * Statements legali
 */
STATEMENT_LIST : STATEMENT_LIST STATEMENT
	       |
	       ;

STATEMENT : ADD_STATEMENT
	  | EXPR_STATEMENT
	  | NULL_STATEMENT
	  | VAR_DECL
	  ;

/* Chiamata di figura con metodo standard */
ADD_STATEMENT : id '=' id '(' 
				{ new_down2($1,$3) ; }
		ARGS_LIST ')'
				{ check_down() ; }
		';'
	      |        id '('
	      			{ new_down1($1) ; }
		ARGS_LIST ')'
				{ check_down() ; }
		';'
	      ;

/* lista argomenti (possibly empty) */
ARGS_LIST : ARGS_LIST1
	  |
	  ;

/* lista argomenti (sicuramente non vuota) */
ARGS_LIST1 : ARG			{}
	   | ARGS_LIST1 ',' ARG		{}
	   ;

/* argomenti di una chiamata */
ARG : id '=' EXPR	{ new_arg_t1($1,$3) ; }
    |        EXPR	{ new_arg_t1_def($1); }
    | id To id '=' EXPR	{ new_arg_t2($1,$3,$5); } 
    | Scale '=' EXPR	{ new_arg_t34($3,3); }
    | Rotate '=' EXPR	{ new_arg_t34($3,4); }
    | Dash '=' EXPR	{ set_dash($3); }
    | Fill '=' EXPR	{ set_fill($3); }
    | string		{ set_label($1); }
    ;

/* Statement nullo */
NULL_STATEMENT: ';'
	      ;

/* Assegnazione a variabile locale */
EXPR_STATEMENT : id '=' EXPR ';' { do_assign( $1.v.s, $3); }
	       ;

EXPR : EXPR '+' EXPR	{ $$ = do_sum( $1, $3 ) ; }
     | EXPR '-' EXPR	{ $$ = do_sub( $1, $3 ) ; }
     | EXPR '*' EXPR	{ $$ = do_mul( $1, $3 ) ; }
     | EXPR '/' EXPR	{ $$ = do_div( $1, $3 ) ; }
     | '(' EXPR ')'	{ $$ = $2 ; }
     | '-' EXPR %prec '*'	{ $$ = do_negate( $2 ) ; }
     | Angle '(' EXPR ')'	{ $$ = do_angle( $3 ) ; }
     | Length '(' EXPR ')'	{ $$ = do_length( $3 ) ; }
     | Versor '(' EXPR ')'	{ $$ = do_versor( $3 ) ; }
     | '[' EXPR ',' EXPR ']'	{ $$ = do_vector( $2,$4 ) ; }
     | id		{ $$ = do_getval( $1 ) ; }
     | id '.' id	{ $$ = do_gethandle( $1, $3) ; }
     | num 		{ $$ = $1; }
     ;

%%

/*
#pragma warn .stv
*/

#include "lex.c"
