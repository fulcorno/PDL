/*
 * DRAW.c
 * Implementa la discesa dell'albero di disegno e chiama il modulo HARD.c
 * per eseguire le primitive
 */

#include "types.h"

typedef struct {
    matrix m ;
    smt_node *ogg ;
    opt_spec opt ;
    int son ;
} stack_node ;

static void s_init PROTO ((void)) ;
static void s_push PROTO(( stack_node *n )) ;
static void s_pop PROTO(( void )) ;
static stack_node *s_peek PROTO((void)) ;

static void traverse PROTO ((void)) ;

#define MAX_DEPTH 40
static int depth ;
static stack_node stack[MAX_DEPTH] ;
static int n_turns ;

void draw_it(dev)
char *dev ;
{
    dev_parm par ;
    smt_node *mogg ;
    spec_item tab[3] ;
    stack_node root ;
 
    /* Calcola la CTM tra main() e device */
    mogg = lookup("main") ;
    if(!mogg) ERROR(E_notdef,"main") ;
    /* T1 bottom */
    tab[0].type = 1 ;
    tab[0].ee = find_handle(mogg,"bottom")->location ;
    /* T1 top */
    tab[1].type = 1 ;
    tab[1].ee = find_handle(mogg,"top")->location ;
    /* T4 0 */
    tab[2].type = 4 ;
    tab[2].val = 0 ;
    
    /* inizializza l'H/W */
    if(!q_flg) printf("\nGenerating output\n") ;
    par.device = dev ;
    par.bot = tab[0].ee ;
    par.top = tab[1].ee ;
    device_init(&par) ;
    tab[0].er = par.bot ;
    tab[1].er = par.top ;
    
    root.m = calc_matrix(tab,3) ;
    root.ogg = mogg ;
    root.son = 0 ;
    root.opt.o_lbl = "" ;
    root.opt.o_fill = 0 ;
    root.opt.o_dash = 0 ;
    s_init() ;
    s_push(&root) ;
    traverse() ;
    device_shut(&par) ;
    if(!q_flg) printf("%d transitions\n", n_turns) ;
} /* end of draw_it() */

static void traverse()
{
    int dn ;
    stack_node *p ;
    down_node *q ;
    stack_node new ;
    dev_exec xc ;
    
    n_turns = 0 ;
    while(p=s_peek())
    {
	n_turns++ ;
	dn = p->ogg->v.obj.down_n ;
	if(dn<0)
	{
	    xc.code = dn ;
	    xc.tm = p->m ;
	    xc.opt = p->opt ;
	    device_exec(&xc) ;
	}
	
	if(dn <= p->son)
	{
	    /* non ha piu` figli ( o era primitiva ) */
	    s_pop() ;
	} else
	{
	    q = & p->ogg->v.obj.down[p->son] ;
	    p->son ++ ;
	    new.son = 0 ;
	    new.ogg = q->ref ;
	    new.m = mult_mm(p->m,q->tm) ;
	    new.opt.o_lbl =
		(q->opt.o_lbl!=NULL) ? q->opt.o_lbl : p->opt.o_lbl ;
	    new.opt.o_fill =
		(q->opt.o_fill!=(-1)) ? q->opt.o_fill : p->opt.o_fill ;
	    new.opt.o_dash =
		(q->opt.o_dash!=(-1)) ? q->opt.o_dash : p->opt.o_dash ;
	    s_push(&new) ;
	}
    }
} /* end of traverse() */

static void s_init()
{
    depth = 0 ;
} /* end of s_init() */

static void s_push (n)
stack_node *n ;
{
    if(depth>=MAX_DEPTH) ERROR(E_nested,NULL) ;
    stack[depth++] = *n ;
} /* end of s_push() */

static void s_pop ()
{
    if(--depth < 0) ERROR(E_internal,"s_pop") ;
} /* end of s_pop() */

static stack_node *s_peek()
{
    if(depth==0) return NULL ;
    else return &stack[depth-1] ;
} /* end of s_peek() */
