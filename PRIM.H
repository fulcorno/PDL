/*
 * prim.h
 * definizione delle primitive del PDL
 */

void g_bgi_init PROTO((dev_parm *p)) ;
void g_ps_init PROTO((dev_parm *p)) ;
void g_x_init PROTO((dev_parm *p)) ;

enum primitive_codes {
	p_NONE,		/*dummy*/
	p_line,		/* segmento [0,0]->[0,1] */
	p_label,	/* scritta centrata in [0,0] */
} ;

typedef struct {
    char *ext ;		/* filename extension */
    void (*g_init) PROTO((dev_parm *p)) ;	/* init function */
} dev_list ;

#ifdef HARD_MODULE
#   define Extern
    dev_list devices[] = {
#if __TURBOC__
	{".bgi", g_bgi_init},
#endif
	{".ps", g_ps_init},
	{NULL, NULL}
    } ;
#else
#define Extern extern
#endif

Extern void (*g_shut) PROTO((void)) ;
Extern void (*g_line) PROTO((point p1, point p2, int dash)) ;
Extern void (*g_label) PROTO((point p, char *lab)) ;

#undef Extern
