/*
 * pdltypes.h
 * Definizioni dei tipi di dato per il linguaggio PDL
 */

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "proto.h"
#include "err.h"


#define TO_DEG(x) ((x)*(57.2957795131))
#define TO_RAD(x) ((x)*(1.74532925199e-2))

/** tipi geometrici **/
typedef struct {
    double sx, sxy, syx, sy, dx, dy ;
} matrix ;

typedef struct {
    double x, y;
} point ;

/** tipi grammaticali **/
#define MAX_ID_LEN 80
union gramnode_v {
    double r ;
    point p ;
    char s[MAX_ID_LEN+1] ;
    int i ;
} ;

enum gramnode_t {
    t_ERR=-1, t_NONE=0, t_scalar, t_point, t_string, t_typeval, t_hattr
} ;

typedef struct {
    enum gramnode_t t ;
    union gramnode_v v ;
} YYStype ;

/** tipi per l'interpretazione **/
typedef struct {
    int valid, scalar ;
    union { double r ; point p ; } v;
} var_ty ;

typedef struct {
    char *o_lbl ;
    int o_dash ;
    int o_fill ;
} opt_spec ;

typedef struct {
    char *name ;
    int attributes ;
    point location ;
} handle_node ;

#define h_DEFAULT 0x01
#define h_DEFVAL  0x02

typedef struct {
    struct smt_node *ref ;
    matrix tm ;
    opt_spec opt ;
} down_node ;

typedef struct {
    int valid ;
    int handle_n ;
    handle_node *handle ;
    int down_n ;
    down_node *down ;
} obj_ty ;

typedef struct {
    int valid ;
    struct smt_node *ref ;
    down_node *down ;
} objref_ty ;

union attr_v {
    var_ty var ;
    obj_ty obj ;
    objref_ty objref ;
} ;

enum attr_t {
    at_ERR=-1, at_NONE=0, at_variable, at_object, at_localvar, at_refobj
} ;

/** tipi per symbol table **/
typedef struct smt_node {
    char *name ;
    enum attr_t t ;
    union attr_v v ;
} smt_node ;

/* tipi per il calcolo */
typedef struct {
    int type ;
    point er , ee ;
    double val ;
} spec_item ;

/***
 *** ESECUZIONE DEL DISEGNO
 ***/

/* informazioni sull'HardWare */
typedef struct {
    char * device ;	/* draw->hard: dove voglio l'output (file) */
    point bot, top ;	/* hard->draw: dimensioni del device */
} dev_parm ;

/* esecuzione di primitive */
typedef struct {
    int code ;		/* quale primitiva */
    matrix tm ;		/* in che spazio eseguirla */
    opt_spec opt ;	/* quali opzioni attivare */
} dev_exec ;

/*
 * Procedure & funzioni
 */

/** PARSE.C **/
/* creazione nodi del parse tree */
YYStype new_str PROTO (( char * val )) ;
YYStype new_num PROTO (( double val )) ;
YYStype new_vec PROTO (( point  val )) ;
/* conversioni */
point make_point PROTO (( double x , double y )) ;
/* interprete delle espressioni */
YYStype do_sum PROTO (( YYStype a, YYStype b )) ;
YYStype do_sub PROTO (( YYStype a, YYStype b )) ;
YYStype do_mul PROTO (( YYStype a, YYStype b )) ;
YYStype do_div PROTO (( YYStype a, YYStype b )) ;
YYStype do_negate PROTO (( YYStype a )) ;
YYStype do_angle PROTO (( YYStype v )) ;
YYStype do_versor PROTO (( YYStype v )) ;
YYStype do_length PROTO (( YYStype v )) ;
YYStype do_vector PROTO (( YYStype a, YYStype b )) ;
YYStype do_getval PROTO (( YYStype a )) ;
YYStype do_gethandle PROTO (( YYStype ref, YYStype tag )) ;
void   	do_assign PROTO (( char *name, YYStype a )) ;

/** MATRIX.C **/
point mult_mv PROTO (( matrix m, point v )) ;
matrix mult_mm PROTO (( matrix m1, matrix m2 )) ;
matrix calc_matrix PROTO (( spec_item *tab, int n_entries )) ;

/** OGG.C **/
handle_node * find_handle PROTO (( smt_node *ref, char *name )) ;
void new_obj PROTO (( YYStype obj_id )) ;
void check_obj PROTO (( YYStype obj_id )) ;
void new_handle PROTO (( YYStype attr, YYStype han_id )) ;
void new_down2 PROTO (( YYStype reference, YYStype callee )) ;
void new_down1 PROTO (( YYStype callee )) ;
void new_arg_t1 PROTO (( YYStype ee, YYStype er )) ;
void new_arg_t1_def PROTO (( YYStype ee )) ;
void new_arg_t2 PROTO (( YYStype ee_from, YYStype ee_to, YYStype er )) ;
void new_arg_t34 PROTO (( YYStype value, int type )) ;
void set_label PROTO (( YYStype label )) ;
void set_dash PROTO (( YYStype type )) ;
void set_fill PROTO (( YYStype type )) ;
void check_down PROTO (( void )) ;
point get_defval PROTO (( smt_node *ref )) ;
point get_val PROTO (( smt_node *ref, char *handle_name )) ;

/** SYMTAB.C **/
smt_node *lookup PROTO (( char *name )) ;
smt_node *create PROTO (( char *name )) ;
smt_node *create_var PROTO (( char *name, int vartype )) ;
smt_node *create_ref PROTO (( char *name )) ;
smt_node *create_obj PROTO (( char *name )) ;
handle_node *create_handle PROTO ((smt_node *obj)) ;
down_node *create_down PROTO ((smt_node *obj)) ;
void table_push PROTO ((void)) ;
void table_pop PROTO ((void)) ;
void smtinit PROTO ((void)) ;
void smt_stat PROTO ((void)) ;
void smt_dump PROTO ((void)) ;

/** BUILT.C **/
void create_builtin PROTO ((void)) ;

/** GRAM.C **/
int yyparse PROTO ((void)) ;

/** MAIN.C **/
enum err_codes ;
int main PROTO (( int argc, char **argv )) ;
void ERROR PROTO (( enum err_codes code, char * info/*opt*/ )) ;
int yywrap PROTO ((void)) ;
void yyerror PROTO (( char *s )) ;

/** HARD.C **/
int device_info PROTO (( void )) ;
int device_init PROTO (( dev_parm * info )) ;
int device_exec PROTO (( dev_exec * order )) ;
int device_shut PROTO (( dev_parm * info )) ;

/** DRAW.C **/
void draw_it PROTO (( char *description )) ;

/* variabili globali */

#ifdef MAIN_MODULE
#   define Extern
#else
#   define Extern extern
#endif

Extern int n_lines ;
Extern int q_flg ;
Extern int d_flg ;
Extern point user_dim ;

#undef Extern
