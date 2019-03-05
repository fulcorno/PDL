/*
 * built.c
 * definisce gli oggetti "built-in" del pdl
 */
 
#include "types.h"
#include "prim.h"

/***
All'atto dell'inizializzazione della symbol table, questi object, costanti,
riferimenti vengono caricati prima di leggere il codice sorgente.
Questo approccio rende indipendente il driver di costruzione delle primitive
dalle primitive effettivamente implementate: nessuna regola grammaticale e`
dedicata alle primitive.
Con una piccola estensione sara` possibile pre-pendere al codice sorgente
delle librerie pre-compilate, basate su queste primitive, che appariranno
all'utente come estensioni naturali del linguaggio.
Il driver di stampa deve essere in grado di eseguire esclusivamente queste
primitive.
***/

static void add_vec PROTO (( char *name, point val )) ;
static void add_vec(s,v)
char *s ;
point v ;
{
    smt_node *p ;
    p = create_var(s,t_point) ;
    p->v.var.valid = 1 ;
    p->v.var.v.p = v ;
} /* end of add_vec() */

static void add_num PROTO (( char *name, double val )) ;
static void add_num(s,v)
char *s ;
double v ;
{
    smt_node *p ;
    p = create_var(s,t_scalar) ;
    p->v.var.valid = 1 ;
    p->v.var.v.r = v ;
} /* end of add_num() */

static void add_obj PROTO (( char *name, int code, int n_handles,
	handle_node *tab )) ;
static void add_obj(s,c,n,tab)
char *s ;
int c,n ;
handle_node *tab ;
{
    smt_node *p ;
    
    p = create_obj(s) ;
    p->v.obj.valid = 1 ;
    p->v.obj.handle_n = n ;
    p->v.obj.down_n = c ;
    p->v.obj.handle = tab ;
} /* end of add_obj() */

static handle_node line_handles[] = {
    { "A", h_DEFAULT, { 0.0, 0.0 } } ,
    { "B", h_DEFAULT|h_DEFVAL, { 1.0, 0.0 } } ,
    { "M", 0,  { 0.5, 0.0 } }
} ;

static handle_node label_handles[] = {
    { "O", h_DEFAULT|h_DEFVAL, {0.0,0.0} }
} ;

void create_builtin()
{

    /* COSTANTI D'USO COMUNE */
    add_vec("_i",make_point(1.0,0.0)) ;
    add_vec("_j",make_point(0.0,1.0)) ;
    
    /* SEGMENTO */
    add_obj("line",-p_line,3,line_handles) ;
    /* TESTO */
    add_obj("label",-p_label,1,label_handles) ;
} /* end of create_builtin() */
