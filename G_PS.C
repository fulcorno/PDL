/*
 * g_ps.c
 * Creazione di files PostScript (tm)
 */

#include "types.h"
#include "prim.h"

void g_ps_shut PROTO((void)) ;
void g_ps_line PROTO((point p1, point p2, int dash)) ;
void g_ps_label PROTO((point p, char *lab)) ;

static void prologue PROTO((dev_parm *p)) ;
static void trailer  PROTO((void)) ;
static void line_flush PROTO((void)) ;

static FILE *psf ;
static int open_path ;
static point last_point ;
static int last_dash ;

/* Dimensioni della pagina (approssimative) */
/* PORTRAIT */
#define DIM_X (d_flg?user_dim.x:8.0)	/*inches*/
#define DIM_Y (d_flg?user_dim.y:11.0)	/*inches*/
#define MARGIN (d_flg?0.0:0.5)	/*inches each side*/
#define FONT_SIZE 18	/*points*/
#define DICT_SIZE 15	/*items*/

#define equal(a,b) (fabs(((b)-(a))/((a)+(b)))<0.0001)

void g_ps_init(p)
dev_parm *p ;
{
    double dimx,dimy, ratio ;
    double windx, windy ;
    char name[255] ;
    
    /* Apre la pagina grafica */
    psf = fopen( p->device, "w" ) ;
    if(!psf) ERROR(E_creat,p->device) ;
    
    /* ora devo decidere in quale area di schermo far comparire il disegno */
    ratio = (DIM_Y-2*MARGIN)/(DIM_X-2*MARGIN) ;
    dimx = p->top.x - p->bot.x ;
    dimy = p->top.y - p->bot.y ;
    if(ratio > dimy/dimx)
    {
	windx = (DIM_X-2*MARGIN)*72.0 ;
	windy = ( windx * dimy ) / dimx ;
    }
    else
    {
	windy = (DIM_Y-2*MARGIN)*72.0 ;
	windx = ( windy * dimx ) / dimy ;
    }
    
    p->top = make_point(MARGIN*72.0+windx,MARGIN*72.0+windy) ;
    p->bot = make_point(MARGIN*72.0,MARGIN*72.0) ;
    
    /* descrive completamente il dispositivo a scopo informativo */
    sprintf(name,"PostScript file \"%s\" (%.2f,%.2f)-(%.2f,%.2f) inches",
       p->device,
       p->bot.x/72.0, p->bot.y/72.0,
       p->top.x/72.0, p->top.y/72.0) ;
    if(NULL==(p->device = strdup(name))) ERROR(E_memory,NULL) ;
    
    /* setta le procedure da usare */
    g_shut = g_ps_shut ;
    g_line = g_ps_line ;
    g_label = g_ps_label ;
    
    /* stampa il prologo */
    prologue(p) ;
    open_path = 0 ;
} /* end of g_ps_init() */

void g_ps_shut()
{
    line_flush() ;
    trailer() ;
    fclose(psf) ;
} /* end of g_ps_shut() */

static void line_flush()
{
    if (open_path)
    {
	fprintf(psf,"d%d s\n", last_dash%8) ;
	open_path = 0 ;
    }
} /* end of line_flush() */

void g_ps_line(a,b,d)
point a,b ;
int d ;
{
    if(open_path && last_dash==d &&
	equal(last_point.x,a.x) &&
	equal(last_point.y,a.y) )
    {
	fprintf(psf,"%.2f %.2f l\n",b.x,b.y) ;
	last_point = b ;
    }
    else if(open_path && last_dash==d &&
	equal(last_point.x,b.x) &&
	equal(last_point.y,b.y) )
    {
	fprintf(psf,"%.2f %.2f l\n",a.x,a.y) ;
	last_point = a ;
    }
    else
    {
	line_flush() ;
	fprintf(psf,"%.2f %.2f m %.2f %.2f l\n",a.x,a.y,b.x,b.y) ;
	open_path = 1 ;
	last_dash = d ;
	last_point = b ;
    }
} /* end of g_ps_line() */

void g_ps_label(p,s)
point p ;
char *s ;
{
    char *t ;
    
    line_flush() ;
    fprintf(psf,"%.2f %.2f m d0 (",p.x,p.y-(FONT_SIZE/2.0)*0.65) ;
    for(t=s; *t; t++)
	switch(*t)
	{
	    case '\n': fprintf(psf,"\\n") ; break ;
	    case '\r': fprintf(psf,"\\r") ; break ;
	    case '\t': fprintf(psf,"\\t") ; break ;
	    case '\b': fprintf(psf,"\\b") ; break ;
	    case '\\': fprintf(psf,"\\\\") ; break ;
	    case '(': fprintf(psf,"\\(") ; break ;
	    case ')': fprintf(psf,"\\)") ; break ;
	    default:
		if( (*t>=0x20) && (*t<=0x7e) )
		    fprintf(psf,"%c",*t) ;
		else fprintf(psf,"\\%03o",*t) ;
	}
    fprintf(psf,") c\n") ;
} /* end of g_ps_label() */

static void prologue(p)
dev_parm *p ;
{
/* header */
    fprintf(psf,"%%!PS-Adobe-2.0 EPSF-2.0\n") ;
    fprintf(psf,"%%%%Title: %s\n",p->device) ;
    fprintf(psf,"%%%%Creator: the Picture Descripton Language PDL %s - Fulvio Corno\n",RELEASE) ;
    fprintf(psf,"%%%%BoundingBox: %.2f %.2f %.2f %.2f\n",
    	p->bot.x,p->bot.y,p->top.x,p->top.y) ;
    fprintf(psf,"%%%%Pages: 0\n") ;
    fprintf(psf,"%%%%DocumentFonts: Courier\n") ;
    fprintf(psf,"%%%%EndComments\n") ;
/* define PDLdict */
    fprintf(psf,"/PDLdict %d dict def PDLdict begin\n",DICT_SIZE) ;
    fprintf(psf,"/s /stroke load def\n") ;
    fprintf(psf,"/m /moveto load def\n") ;
    fprintf(psf,"/l /lineto load def\n") ;
    
    fprintf(psf,"/d0 { [] 0 setdash 1 setlinewidth } bind def\n") ;
    fprintf(psf,"/d1 { [3 9] 0 setdash 1 setlinewidth } bind def\n") ;
    fprintf(psf,"/d2 { [9 6 3 6] 0 setdash 1 setlinewidth } bind def\n") ;
    fprintf(psf,"/d3 { [9] 0 setdash 1 setlinewidth } bind def\n") ;
    fprintf(psf,"/d4 { [] 0 setdash 3 setlinewidth } bind def\n") ;
    fprintf(psf,"/d5 { [3 9] 0 setdash 3 setlinewidth } bind def\n") ;
    fprintf(psf,"/d6 { [9 6 3 6] 0 setdash 3 setlinewidth } bind def\n") ;
    fprintf(psf,"/d7 { [9] 0 setdash 3 setlinewidth } bind def\n") ;

    fprintf(psf,"/c { dup stringwidth pop 2 div neg 0 rmoveto show } bind def\n") ;
    fprintf(psf,"end\n") ;
    fprintf(psf,"%%%%EndProlog\n") ;
/* Execute setup */
    fprintf(psf,"%%%%BeginSetup\n") ;
    fprintf(psf,"save\n") ;
    fprintf(psf,"PDLdict begin\n") ;
    fprintf(psf,"/Courier findfont %d scalefont setfont\n",FONT_SIZE) ;
    fprintf(psf,"2 setlinecap 0 setlinejoin\n") ;
    fprintf(psf,"%%%%EndSetup\n") ;
    fprintf(psf,"%%%%Page: unique 1\n") ;
} /* end of prologue() */

static void trailer()
{
    fprintf(psf,"end restore showpage\n") ;
    fprintf(psf,"%%%%Trailer\n") ;
} /* end of trailer() */
