/*
 * g_bgi.c
 * Esecuzione delle primitive da parte dell'unita` BGI
 */

#include "types.h"
#include "prim.h"
#include <graphics.h>

void g_bgi_shut PROTO((void)) ;
void g_bgi_line PROTO((point p1, point p2, int dash)) ;
void g_bgi_label PROTO((point p, char *lab)) ;

static int lstyle ;

void g_bgi_init(p)
dev_parm *p ;
{
    int gmode1, gmode2, gdev, err ;
    int ax,ay, col0, col1 ;
    double dimx,dimy, ratio ;
    double windx, windy ;
    int x1,x2,y1,y2 ;
    char name[255] ;
    
    gdev = -1 ;
    /* testa i device supportati */
    if(!strcmp(p->device,"cga.bgi")) gdev=CGA ;
    if(!strcmp(p->device,"mcga.bgi")) gdev=MCGA ;
    if(!strcmp(p->device,"ega.bgi")) gdev=EGA ;
    if(!strcmp(p->device,"ega64.bgi")) gdev=EGA64 ;
    if(!strcmp(p->device,"egamono.bgi")) gdev=EGAMONO ;
    if(!strcmp(p->device,"8514.bgi")) gdev=IBM8514 ;
    if(!strcmp(p->device,"herc.bgi")) gdev=HERCMONO ;
    if(!strcmp(p->device,"att.bgi")) gdev=ATT400 ;
    if(!strcmp(p->device,"vga.bgi")) gdev=VGA ;
    if(!strcmp(p->device,"3270.bgi")) gdev=PC3270 ;
    if(gdev==-1) ERROR(E_inv_device,p->device) ;
    
    /* trova il modo a maggior risoluzione */
    getmoderange(gdev, &gmode1,&gmode2) ;
    
    /* Apre la pagina grafica */
    initgraph(&gdev,&gmode2,getenv("BGI")) ;
    err = graphresult() ;
    if (err != grOk) ERROR(E_init_dev,grapherrormsg(err)) ;
    
    /* descrive completamente il dispositivo a scopo informativo */
    strcpy(name,"Device=\"") ;
    strcat(name,getdrivername()) ;
    strcat(name,"\", Mode=\"") ;
    strcat(name,getmodename(getgraphmode())) ;
    strcat(name,"\"") ;
    if(NULL==(p->device = strdup(name))) ERROR(E_memory,NULL) ;
    
    /* ora devo decidere in quale area di schermo far comparire il disegno */
    getaspectratio(&ax,&ay) ;
    ratio = ((double)ay*(double)getmaxy()) / ((double)ax*(double)getmaxx()) ;
    dimx = p->top.x - p->bot.x ;
    dimy = p->top.y - p->bot.y ;
    if(ratio > dimy/dimx)
    {
	windx = getmaxx() ;
	windy = ( windx * (dimy/ay) ) / (dimx/ax) ;
    }
    else
    {
	windy = getmaxy() ;
	windx = ( windy * (dimx/ax) ) / (dimy/ay) ;
    }
    x1 = (getmaxx()-windx)/2 ;
    x2 = getmaxx() - x1 ;
    y1 = (getmaxy()-windy)/2 ;
    y2 = getmaxy() - y1 ;
    
    p->top = make_point((double)(x2-x1),0.0) ;
    p->bot = make_point(0.0,(double)(y2-y1)) ;
    
    /* sceglie i colori con cui disegnare */
    col0 = getmaxcolor() ;
    col1 = getbkcolor() ;
    
    /* disegna lo sfondo per il disegno */
    setcolor(col0) ;
    setfillstyle(CLOSE_DOT_FILL,col0) ;
    bar(0,0,getmaxx(),getmaxy()) ;
    setviewport(x1,y1,x2,y2,1) ;
    setfillstyle(SOLID_FILL,col0) ;
    bar(0,0,x2-x1,y2-y1) ;
    setcolor(col1) ;
    
    /* setta le procedure da usare */
    g_shut = g_bgi_shut ;
    g_line = g_bgi_line ;
    g_label = g_bgi_label ;
    
    /* setta i default */
    lstyle = -1 ; /* cosi` la prima volta se la setta */
} /* end of g_bgi_init() */

void g_bgi_shut()
{
    (void)getchar() ;
    closegraph() ;
} /* end of g_bgi_shut() */

void g_bgi_line(a,b,d)
point a,b ;
int d ;
{
    if(d!=lstyle)
    {
	setlinestyle(d%4,0,3*(d/4)) ;
	lstyle = d ;
    }
    line(a.x,a.y,b.x,b.y) ;
} /* end of g_bgi_line() */

void g_bgi_label(p,s)
point p ;
char *s ;
{
    settextjustify(CENTER_TEXT,CENTER_TEXT) ;
    settextstyle(DEFAULT_FONT,HORIZ_DIR,1) ;
    outtextxy(p.x,p.y,s) ;
} /* end of g_bgi_label() */
