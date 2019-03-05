/*
 * ogg.c
 * routines per la definizione, il controllo e la chiamata di oggetti
 * definiti dall'utente.
 */

#include "types.h"

/* dati sull'oggetto in fase di creazione */
static smt_node *curr_obj=NULL ;
static seen_defval ;

/* dati sull'oggetto in fase di chiamata */
static smt_node *curr_callee=NULL ;
static smt_node *curr_refer=NULL ;

/* dati sulla lista di parametri in esame */
#define MAX_SPEC_NUM 6
static int param_num ;
static int last_default ;
static spec_item spec_tab[MAX_SPEC_NUM+1] ;
static opt_spec curr_opt ;

/* PROCEDURE DI SERVIZIO */

handle_node * find_handle ( ref, name )
smt_node *ref ;
char *name ;
{
    int i,found ;
    handle_node *q ;
    
    if(ref->t != at_object) ERROR(E_internal,"find_handle1") ;
    if(!ref->v.obj.valid) ERROR(E_notval_obj,ref->name) ;
    for(i=found=0; i<ref->v.obj.handle_n; ++i)
    {
	q = & ref->v.obj.handle[i] ;
	if(!name && (q->attributes & h_DEFVAL))
	    { found = 1 ; break ; }
	else if(name && !strcmp(name,q->name))
	    { found = 1 ; break ; }
    }
    if(!found)
    {
	if(name) ERROR(E_nonex_handle,name) ;
	else ERROR(E_nonex_defval,NULL) ;
    }
    return q ;
} /* end of find_handle() */

/* CREAZIONE DI OGGETTI NUOVI */
void new_obj( a )
YYStype a ;
{
    if(curr_obj!= NULL) ERROR(E_internal,"new_obj2") ;
    if(a.t != t_string) ERROR(E_internal,"new_obj1") ;
    curr_obj = create_obj(a.v.s) ;
    seen_defval = 0 ;
    table_push() ;
} /* end of new_obj() */

void check_obj( a )
YYStype a ;
{
    int i ;
    smt_node *q ;
    handle_node *p ;
    
    if (curr_obj == NULL) ERROR(E_internal,"check_obj2") ;
    if (a.t != t_string) ERROR(E_internal,"check_obj1") ;
    if (strcmp(a.v.s, curr_obj->name)) ERROR(E_decl_msm,curr_obj->name) ;
    /* assegna agli handle il loro valore */
    for(i=0; i< curr_obj->v.obj.handle_n; ++i)
    {
	p = & curr_obj->v.obj.handle[i] ;
	q = lookup(p->name) ;
	if((q->t != at_localvar) || (q->v.var.scalar))
	    ERROR(E_internal,"check_obj3") ;
	if(q->v.var.valid)
	    p->location = q->v.var.v.p ;
	else ERROR(E_undef_loc, p->name) ;
    }
    /* esce di scope */
    table_pop() ;
    curr_obj->v.obj.valid = 1 ;
    curr_obj = NULL ;
} /* end of check_obj() */

void new_handle( a, b )
YYStype a,b ;
{
    handle_node *p ;
    smt_node *q ;
    
    if(a.t != t_hattr) ERROR(E_internal,"new_handle1") ;
    if(b.t != t_string) ERROR(E_internal,"new_handle2") ;
    
    p = create_handle(curr_obj) ;
    q = create_var(b.v.s, t_point) ;
    if( NULL ==(p->name = strdup(q->name)) ) ERROR(E_memory,NULL) ;
    if (a.v.i & h_DEFVAL)
	if(seen_defval) ERROR(E_defval_dup,p->name) ;
	else seen_defval = 1 ;
    p->attributes = a.v.i ;
} /* end of new_handle() */


/* CHIAMATA DI OGGETTI ESISTENTI */
void new_down1(b)
YYStype b ;
{
    if(!curr_obj) ERROR(E_internal,"new_down1.3") ;
    if(curr_callee) ERROR(E_internal,"new_down1.1") ;
    if(b.t!=t_string) ERROR(E_internal,"new_down1.2") ;
    curr_callee = lookup(b.v.s) ;
    if(!curr_callee) ERROR(E_notdef_obj,b.v.s) ;
    if(curr_callee->t != at_object ) ERROR(E_notdef_obj,b.v.s) ;
    if(!curr_callee->v.objref.valid) ERROR(E_notval_obj,b.v.s) ;
    param_num = 0 ;
    last_default = 0 ;
    curr_refer = NULL ;
    curr_opt.o_lbl = NULL ;
    curr_opt.o_dash = -1 ;
    curr_opt.o_fill = -1 ;
} /* end of new_down1() */

void new_down2(a,b)
YYStype a,b ;
{
    smt_node *p ;
    
    new_down1(b) ;
    if(a.t!=t_string) ERROR(E_internal,"new_down2.1") ;
    p=create_ref(a.v.s) ;
    curr_refer = p ;
} /* end of new_down2() */

void set_label(a)
YYStype a ;
{
    if(curr_opt.o_lbl) ERROR(E_dupl_opt,"label") ;
    if(a.t != t_string) ERROR(E_internal,"set_label1") ;
    if(NULL==(curr_opt.o_lbl=strdup(a.v.s))) ERROR(E_memory,NULL) ;
} /* end of set_label() */

void set_fill(a)
YYStype a ;
{
    if(curr_opt.o_fill != -1) ERROR(E_dupl_opt,"fill") ;
    if(a.t != t_scalar) ERROR(E_inv_t_op,"fill") ;
    if(a.v.r < 0) ERROR(E_inv_opt,"fill") ;
    curr_opt.o_fill = (int)a.v.r ;
} /* end of set_fill() */

void set_dash(a)
YYStype a ;
{
    if(curr_opt.o_dash != -1) ERROR(E_dupl_opt,"fill") ;
    if(a.t != t_scalar) ERROR(E_inv_t_op,"dash") ;
    if(a.v.r < 0) ERROR(E_inv_opt,"dash") ;
    curr_opt.o_dash = (int)a.v.r ;
} /* end of set_dash() */

void check_down()
{
    down_node *p ;
    
    p = create_down(curr_obj) ;
    p->tm = calc_matrix(spec_tab,param_num) ;
    p->ref = curr_callee ;
    p->opt = curr_opt ;
    if(curr_refer) 
    {
	curr_refer->v.objref.ref = curr_callee ;
	curr_refer->v.objref.down = p ;
	curr_refer->v.objref.valid = 1 ;
	curr_refer=NULL ;
    }
    curr_callee = NULL ;
} /* end of check_down() */

void new_arg_t1 ( a, b ) 
YYStype a, b ;
{
    spec_tab[param_num].type = 1 ;
    if(a.t!=t_string) ERROR(E_internal,"new_arg_t1.1") ;
    if(b.t!=t_point) ERROR(E_inv_t_op,"=") ;
    spec_tab[param_num].ee = find_handle(curr_callee,a.v.s)->location ;
    spec_tab[param_num].er = b.v.p ;
    if(++param_num>MAX_SPEC_NUM) ERROR(E_many_param,NULL) ;
} /* end of new_arg_t1() */
    
void new_arg_t1_def ( b ) 
YYStype b ;
{
    spec_tab[param_num].type = 1 ;
    if(b.t!=t_point) ERROR(E_inv_t,NULL) ;
    while(last_default <= curr_callee->v.obj.handle_n)
    {
	if(curr_callee->v.obj.handle[last_default].attributes & h_DEFAULT)
	    break ;
	last_default ++ ;
    }
    if(last_default >= curr_callee->v.obj.handle_n)
	ERROR(E_many_def,curr_callee->name) ;
    spec_tab[param_num].ee=curr_callee->v.obj.handle[last_default++].location ;
    spec_tab[param_num].er=b.v.p ;
    if(++param_num>MAX_SPEC_NUM) ERROR(E_many_param,NULL) ;
} /* end of new_arg_t1() */
    
void new_arg_t2 ( a1, a2, b ) 
YYStype a1, a2, b ;
{
    point p1, p2 ;
    spec_tab[param_num].type = 2 ;
    if(a1.t!=t_string) ERROR(E_internal,"new_arg_t2.1") ;
    if(a2.t!=t_string) ERROR(E_internal,"new_arg_t2.2") ;
    if(b.t!=t_point) ERROR(E_inv_t_op,"=") ;
    p1 = find_handle(curr_callee,a1.v.s)->location ;
    p2 = find_handle(curr_callee,a2.v.s)->location ;
    spec_tab[param_num].ee.x = p2.x - p1.x ;
    spec_tab[param_num].ee.y = p2.y - p1.y ;
    spec_tab[param_num].er = b.v.p ;
    if(++param_num>MAX_SPEC_NUM) ERROR(E_many_param,NULL) ;
} /* end of new_arg_t1() */
    
void new_arg_t34 ( a, t ) 
YYStype a ;
int t ;
{
    if(t!=3 && t!=4) ERROR(E_internal,"new_arg_t34.1") ;
    spec_tab[param_num].type = t ;
    if(a.t!=t_scalar) ERROR(E_internal,"new_arg_t34.2") ;
    spec_tab[param_num].val = a.v.r ;
    if(++param_num>MAX_SPEC_NUM) ERROR(E_many_param,NULL) ;
} /* end of new_arg_t34() */
    

/* COORDINATE DI handle DI OGGETTI GIA` CHIAMATI */
point get_defval(ref)
smt_node *ref ;
{
    return get_val(ref,NULL) ;
} /* end of get_defval() */

point get_val(ref,tag)
smt_node *ref ;
char *tag ;
{
    smt_node *p ;
    handle_node *q ;
    down_node *d ;
    
    if(!curr_obj) ERROR(E_internal, "get_val4") ;
    if(ref->t != at_refobj) ERROR(E_internal,"get_val1") ;
    if(!ref->v.objref.valid) ERROR(E_notval_ref,ref->name) ;
    p = ref->v.objref.ref ;
    q = find_handle(p,tag) ;
    /* devo riportare il punto nel sist. di rif. locale */
    /* prima trovo il nodo 'down' corrispondente */
    d = ref->v.objref.down ;
    return mult_mv(d->tm,q->location) ;
} /* end of get_val() */
