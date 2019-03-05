/*
 * symtab.c
 * Gestore delle tabella dei simboli per il linguaggio PDL.
 */

#include "types.h"

/*
 * Implementation section: local definitions
 */

#define HASH_SIZE 211
#define HANDLE_POOL_SIZE 300
#define DOWN_POOL_SIZE   300

typedef struct hash_node {
  smt_node data ;
  struct hash_node * next ;
} hash_node ;

static hash_node *htable[HASH_SIZE] ;
static hash_node *ht_old[HASH_SIZE] ;
static int Global = 1 ;

/* pool di nodi per la definizione oggetti */
static handle_node handlepool [HANDLE_POOL_SIZE] ;
static down_node   downpool   [DOWN_POOL_SIZE]   ;
static int handlefree = 0 ;
static int downfree = 0 ;
static int n_hdl=0, n_dwn=0, n_obj=0, n_ref=0, n_var=0, n_lvr=0 ;

static int Hfun PROTO (( char *key )) ;
static int Hfun (key)
char *key ;
{
  ulong h=0, g ;

  for (; *key; key++){
    h <<= 4 ;
    h += *key ;
    if ((g = h & 0xF0000000L) != 0)
      h ^= (g>>24)^g ;
  } /* endfor */
  return (int)(h % HASH_SIZE) ;
} /* end of Hfun() */

/*
 * Interface section: public function definition
 */

smt_node * lookup ( name )
char *name ;
{
    hash_node *p = htable[Hfun(name)];

    while(p && strcmp(name,p->data.name)) p=p->next ;
    return p ? &p->data : NULL ;
} /* end of lookup() */

smt_node * create ( name )
char *name ;
{
  if (lookup(name)!=NULL)
    ERROR(E_dupl_sym, name) ;  /* c'era gia` */
  else {
    hash_node *q ;
    int i = Hfun(name) ;

    q = (hash_node*) malloc(sizeof(hash_node)) ;
    if(!q) ERROR(E_memory,NULL) ;
    q->next = htable[i] ;
    htable[i] = q ;
    if(NULL==(q->data.name = strdup(name))) ERROR(E_memory,NULL) ;
    q->data.t = at_NONE ;
    return &q->data ;
  }
  return NULL ;
} /* end of create() */

smt_node *create_var ( name, vartype )
char *name ;
int vartype ;
{
    smt_node *p = create (name) ;
    
    if(Global) if(!q_flg) printf("(%s) ",name) ;
    if(Global) n_var++ ; else n_lvr++ ;
    p->t = Global ? at_variable : at_localvar ;
    p->v.var.valid=0 ;
    p->v.var.scalar = (vartype == t_scalar) ;
    
    return p ;
} /* end of create_var() */

smt_node *create_ref ( name )
char *name ;
{
    smt_node *p = create (name) ;
    
    if(Global) ERROR(E_internal,"create_ref1") ; 
    p->t = at_refobj ;
    p->v.var.valid=0 ;
    n_ref++ ;
    
    return p ;
} /* end of create_ref() */

smt_node *create_obj ( name )
char *name ;
{
    smt_node *p = create (name) ;
    
    p->t = at_object ;
    p->v.obj.valid = 0 ;
    p->v.obj.handle_n = 0 ;
    p->v.obj.handle = & handlepool[handlefree] ;
    p->v.obj.down_n = 0 ;
    p->v.obj.down = & downpool[downfree] ;
    n_obj++ ;
    if(!q_flg) printf("[%s] ", name) ;
    
    return p ;
} /* end of create_obj() */

handle_node * create_handle (p)
smt_node *p ;
{
    if(handlefree++ >= HANDLE_POOL_SIZE) ERROR(E_handle_full,NULL) ;
    n_hdl++ ;
    return & p->v.obj.handle[p->v.obj.handle_n++] ;
} /* end of create_handle() */

down_node * create_down (p)
smt_node *p ;
{
    if(downfree++ >= DOWN_POOL_SIZE) ERROR(E_down_full,NULL) ;
    n_dwn++ ;
    return & p->v.obj.down[p->v.obj.down_n++] ;
} /* end of create_down() */

void smtinit()
{
    memset(htable,0,HASH_SIZE*sizeof(hash_node*)) ;
    memset(ht_old,0,HASH_SIZE*sizeof(hash_node*)) ;
    Global = 1 ;
} /* end of smtinit() */

void table_push()
{
    if(!Global) ERROR(E_internal,"table_push1") ;
    memcpy(ht_old, htable, HASH_SIZE * sizeof(hash_node*));
    Global = 0 ;
} /* end of table_push() */

void table_pop()
{
    hash_node **p, *q1, **q ;
    int i ;
    if(Global) ERROR(E_internal,"table_pop1") ;
    for (i=0, p=ht_old, q=htable; i<HASH_SIZE; ++i, ++p, ++q)
    {
	while(*p != *q)
	{
	    free((*q)->data.name) ;
	    q1 = (*q)->next ;
	    free(*q) ;
	    *q = q1 ;
	}
    }
    Global = 1 ;
} /* end of table_pop() */

void smt_stat()
{
    if(!q_flg) printf("%d objects - %d variables - %d locals\n",n_obj,n_var,n_lvr) ;
    if(!q_flg) printf("%d references - %d handles - %d downs\n",n_ref,n_hdl,n_dwn) ;
} /* end of smt_stat() */
	    
#ifdef SMTDEBUG

void smt_dump()
{
    int i ;
    hash_node *q ;
    smt_node *p ;
    
    printf("SYMBOL TABLE:\n") ;
    for (i=0; i<HASH_SIZE; ++i)
       for(q=htable[i] ; q ; q = q->next)
       {
	  p = & q->data ;
          printf("\"%s\" ",p->name) ;
	  switch(p->t)
	  {
	     case at_ERR:
	        printf("ERROR") ; break ;
	     case at_NONE:
	        printf("NO TYPE") ; break ;
	     case at_variable:
	     case at_localvar:
	        printf("%s %s variable",
		   (p->t==at_variable) ? "global":"local",
		   (p->v.var.scalar) ? "scalar":"vector" ) ;
		if(p->v.var.valid)
		{
		   if(p->v.var.scalar) printf(" =%f",p->v.var.v.r ) ;
		   else printf(" =[%f,%f]",p->v.var.v.p.x,p->v.var.v.p.y) ;
		} else printf(" invalid") ;
		break ;
	     case at_refobj:
		if(p->v.objref.valid) printf("reference to object \"%s\"",
		   p->v.objref.ref->name) ;
		else printf("invalid object reference") ;
		break ;
	     case at_object:
		 if(p->v.obj.valid) {
		   int i ;
		   handle_node *q ;
		   down_node *q1 ;
		   for(i=0; i<p->v.obj.handle_n; ++i)
		   { 
		      q = & p->v.obj.handle[i] ;
		      printf("\n\t'%s' [%f,%f]%s%s",q->name, q->location.x,
		      q->location.y, (q->attributes&h_DEFAULT)?" default":"",
		      (q->attributes&h_DEFVAL) ? " defval":"");
		   }
		   if(p->v.obj.down_n<0) printf("\n\t\t<PRIMITIVE> code (%d)",
		       p->v.obj.down_n) ;
		   else for(i=0; i<p->v.obj.down_n; ++i)
		   {
		       q1 = & p->v.obj.down[i] ;
		       printf("\n\t[[%f %f %f]\n\t [%f %f %f]] '%s'",
		       q1->tm.sx, q1->tm.sxy, q1->tm.dx,
		       q1->tm.syx, q1->tm.sy, q1->tm.dy,
		       q1->ref->name ) ;
		   }
		 } else printf("invalid object") ;
		 break ;
	     default:
		ERROR(E_internal,"smt_dump1") ;
		break ;
	  }
	  printf("\n") ;
       }
} /* end of smt_dump() */
#else
void smt_dump(){}
#endif
