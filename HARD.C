/*
 * HARD.c
 * driver del PDL che si occupa di interagire con l'hardware.
 * E` l'unica parte del programma conscia del dispositivo su cui andra` generato
 * il disegno. Comunica con DRAW.c.
 */
#define HARD_MODULE
#include "types.h"
#include "prim.h"

static int n_lines, n_labels ;

int device_init( p )
dev_parm *p ;
{
    char *s ;
    dev_list *dp ;
    
    g_shut = NULL ;
    g_line = NULL ;
    g_label = NULL ;
    
    s = strrchr(p->device,'.') ;
    if(s==NULL) ERROR(E_miss_device,NULL) ;
    else
    {
    	dp = devices ;
	while(dp->ext)
	{
	    if(!strcmp(s,dp->ext))
	    {
		dp->g_init(p) ;
		break ;
	    }
	    dp++ ;
	}
    }
	
    if(!dp->ext) ERROR(E_inv_device,p->device) ;
    if(!g_shut || !g_line || !g_label) ERROR(E_low_device,p->device) ;
    n_lines = n_labels = 0 ;
    return 0 ;
} /* end of device_init() */

int device_shut(p)
dev_parm *p ;
{
    g_shut() ;
    if(!q_flg) printf("output terminated - %s\n",p->device) ;
    if(!q_flg) printf("%d lines - %d labels\n", n_lines, n_labels) ;
    return 0;
}

int device_exec(p)
dev_exec *p ;
{
    switch(-p->code)
    {
	case p_NONE: return 0 ;
	case p_line:
	    n_lines++ ;
	    g_line(mult_mv(p->tm,make_point(0.0,0.0)),
	           mult_mv(p->tm,make_point(1.0,0.0)),
		   p->opt.o_dash) ;
	    return 0 ;
	case p_label:
	    if(*p->opt.o_lbl)
	    {
		n_labels ++ ;
		g_label(mult_mv(p->tm,make_point(0.0,0.0)), p->opt.o_lbl) ;
	    }
	    return 0 ;
	default: ERROR(E_internal,"device_exec") ;
	    return 1 ;
    }
} /* end of device_exec() */

int device_info ()
{
    dev_list *dp ;
    for(dp=devices; dp->ext; ++dp)
	printf(" %s",dp->ext) ;
    return 0 ;
}
