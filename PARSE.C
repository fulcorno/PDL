/*
 * parse.c
 * routines chiamate da scanner & parser per la costruzione
 * dell'albero semantico
 */

#include "types.h"

/*
 * Creazione di nuovi nodi
 */
YYStype new_str ( val )
char *val ;
{
    YYStype node ;

    node.t = t_string ;
    strcpy(node.v.s, val) ;
    return node ;
} /* end of new_str */

YYStype new_num ( val )
double val ;
{
    YYStype node ;

    node.t = t_scalar ;
    node.v.r = val ;
    return node ;
} /* end of new_num */

YYStype new_vec ( val )
point val ;
{
    YYStype node ;

    node.t = t_point ;
    node.v.p = val ;
    return node ;
} /* end of new_vec */

/*
 * Conversioni di tipo implicite
 */
point make_point ( x,y )
double x,y ;
{
    point ret ;

    ret.x = x ;
    ret.y = y ;
    return ret ;
} /* end of make_point */

/*
 * Operazioni aritmetico-matematiche
 */

YYStype do_sum ( a,b )
YYStype a,b ;
{
    YYStype ret ;

    ret.t = t_ERR ;
    if(a.t == t_scalar && b.t == t_scalar) {
	ret.t = t_scalar ;
	ret.v.r = a.v.r + b.v.r ;
    } else if(a.t == t_point && b.t == t_point) {
	ret.t = t_point ;
	ret.v.p = make_point( a.v.p.x+b.v.p.x, a.v.p.y+b.v.p.y ) ;
    } else ERROR(E_inv_t_op,"+") ;

    return ret ;
} /* end of do_sum */

YYStype do_sub ( a,b )
YYStype a,b ;
{
    YYStype ret ;

    ret.t = t_ERR ;
    if(a.t == t_scalar && b.t == t_scalar) {
	ret.t = t_scalar ;
	ret.v.r = a.v.r - b.v.r ;
    } else if(a.t == t_point && b.t == t_point) {
	ret.t = t_point ;
	ret.v.p = make_point( a.v.p.x-b.v.p.x, a.v.p.y-b.v.p.y ) ;
    } else ERROR(E_inv_t_op,"-") ;

    return ret ;
} /* end of do_sub */

YYStype do_mul ( a,b )
YYStype a,b ;
{
    YYStype ret ;

    ret.t = t_ERR ;
    if(a.t == t_scalar && b.t == t_scalar) {
	ret.t = t_scalar ;
	ret.v.r = a.v.r * b.v.r ;
    } else if(a.t == t_point && b.t == t_point) {
	ret.t = t_scalar ;
	ret.v.r = a.v.p.x*b.v.p.x + a.v.p.y*b.v.p.y ;
    } else if(a.t == t_scalar && b.t == t_point) {
	ret.t = t_point ;
	ret.v.p = make_point( a.v.r * b.v.p.x, a.v.r * b.v.p.y ) ;
    } else if(a.t == t_point && b.t == t_scalar) {
	ret.t = t_point ;
	ret.v.p = make_point( a.v.p.x * b.v.r, a.v.p.y * b.v.r ) ;
    } else ERROR(E_inv_t_op,"*") ;

    return ret ;
} /* end of do_mul */

YYStype do_div ( a,b )
YYStype a,b ;
{
    YYStype ret ;

    ret.t = t_ERR ;
    if(a.t == t_scalar && b.t == t_scalar) {
	ret.t = t_scalar ;
	ret.v.r = a.v.r / b.v.r ;
    } else if(a.t == t_point && b.t == t_scalar) {
	ret.t = t_point ;
	ret.v.p = make_point( a.v.p.x/b.v.r, a.v.p.y/b.v.r ) ;
    } else ERROR(E_inv_t_op,"/") ;

    return ret ;
} /* end of do_div */

YYStype do_negate ( a )
YYStype a ;
{
    YYStype ret ;
    
    ret.t = t_ERR ;
    if(a.t == t_scalar) {
	ret.t = t_scalar ;
	ret.v.r = - a.v.r ;
    } else if(a.t == t_point) {
	ret.t = t_point ;
	ret.v.p = make_point( -a.v.p.x, -a.v.p.y ) ;
    } else ERROR(E_inv_t_op,"unary -") ;
    
    return ret ;
} /* end of do_negate() */

YYStype do_angle ( v )
YYStype v ;
{
    YYStype ret ;
    double t ;

    ret.t = t_ERR ;
    if ( v.t == t_point ) {
	ret.t = t_scalar ;
	t = TO_DEG(atan2( v.v.p.y, v.v.p.x )) ;
	if(t<0.0) t += 360.0 ; /* restrict to 0..360 */
	ret.v.r = t ;
    } else ERROR(E_inv_t_op,"angle(v)") ;

    return ret ;
}

YYStype do_versor ( v )
YYStype v ;
{
    YYStype ret ;

    ret.t = t_ERR ;
    if ( v.t == t_scalar ) {
	ret.t = t_point ;
	ret.v.p.x = cos(TO_RAD(v.v.r)) ;
	ret.v.p.y = sin(TO_RAD(v.v.r)) ;
    } else ERROR(E_inv_t_op,"versor(v)") ;

    return ret ;
} /* end of do_versor */

YYStype do_length ( v )
YYStype v ;
{
    YYStype ret ;

    ret.t = t_ERR ;
    if( v.t == t_point ) {
	ret.t = t_scalar ;
	ret.v.r = sqrt(v.v.p.x * v.v.p.x + v.v.p.y * v.v.p.y) ;
    } else ERROR(E_inv_t_op,"length(v)") ;

    return ret ;
} /* end of do_length */

YYStype do_vector ( a,b )
YYStype a,b ;
{
    YYStype ret ;

    ret.t = t_ERR ;
    if( a.t == t_scalar && b.t == t_scalar ) {
	ret.t = t_point ;
	ret.v.p = make_point( a.v.r, b.v.r ) ;
    } else ERROR( E_inv_t_op, "[x,y]") ;

    return ret ;
} /* end of do_vector */

YYStype do_getval ( a )
YYStype a ;
{
    YYStype ret ;
    smt_node *p ;
    
    ret.t = t_ERR ;
    
    if (a.t==t_string)
    {
	/* lookup for identifier in symbol table */
	p = lookup(a.v.s) ;
	if(!p) ERROR(E_notdef,a.v.s) ;
	switch(p->t)
	{
	    case at_refobj:
		ret.t = t_point ;
		ret.v.p = get_defval(p) ;
	    break ;
	    case at_variable:
	    case at_localvar:
		if(p->v.var.valid)
		{
		    /* valid variable */
		    if (p->v.var.scalar) {
		      ret.t = t_scalar ;
		      ret.v.r = p->v.var.v.r ;
		    } else {
		      ret.t = t_point ;
		      ret.v.p = p->v.var.v.p ;
		    }
		} else ERROR(E_not_init_var,a.v.s) ; /* invalid reference */
	    break ;
	    default: ERROR(E_inv_t,a.v.s) ;
	}
    } else ERROR(E_internal,"do_getval1") ;
    
    return ret ;
} /* end of do_getval() */

YYStype do_gethandle( a, b )
YYStype a, b ;
{
    YYStype ret ;
    smt_node *p ;
    
    ret.t = t_ERR ;
    if( (a.t!=t_string) || (b.t!=t_string) ) ERROR(E_internal,"do_gethandle1");
    p = lookup(a.v.s) ;
    if(!p) ERROR(E_notdef,a.v.s) ;
    if(p->t != at_refobj) ERROR(E_inv_t,p->name) ;
    ret.t = t_point ;
    ret.v.p = get_val(p,b.v.s) ;
    return ret ;
} /* end of do_gethandle() */

void do_assign ( name, a )
char *name ;
YYStype a ;
{
  smt_node *p = lookup(name) ;

  if(!p) ERROR(E_notdef,name) ;
  else if( (p->t!=at_variable) && (p->t!=at_localvar) )
     ERROR(E_inv_t,name) ;
  else if(p->v.var.scalar) {
     if (a.t != t_scalar) ERROR(E_inv_t,name) ;
     else {
	p->v.var.valid = 1 ;
	p->v.var.v.r = a.v.r ;
     }
  } else {
     if (a.t != t_point) ERROR(E_inv_t,name) ;
     else {
	p->v.var.valid = 1 ;
	p->v.var.v.p = a.v.p ;
     }
  }
} /* end of do_assign() */

