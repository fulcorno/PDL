/*
 * MATRIX.C
 * procedure che fanno uso del calcolo matriciale per dedurre le coordinate
 * dei punti.
 */

#include "types.h"

/* routines di servizio */
static double pitagora PROTO (( point a )) ;
static point delta PROTO (( point p2, point p1 )) ;
static void ruota PROTO (( matrix *m, point dee, point der )) ;
	/* calcola m->s* a partire da una coppia di vettori liberi */
static void rotscal PROTO (( matrix *m, double scale, double angle )) ;
	/* ruota di un angolo in radianti */
static void trasla PROTO (( matrix *m, point ee, point er )) ;
	/* calcola m->d* a partire da m->s* e una coppia di punti */
static void sort_specs PROTO (( spec_item *table, int n_items )) ;

static void check_matrix PROTO (( matrix m, spec_item *p, int num )) ;

static double pitagora(a)
point a;
{
    return a.x*a.x + a.y*a.y ;
} /* end of pitagora() */

static point delta (p2,p1)
point p2,p1 ;
{
    point ret ;
    ret.x = p2.x-p1.x ;
    ret.y = p2.y-p1.y ;
    return ret ;
} /* end of delta() */

static void ruota(m,dee,der)
matrix *m ;
point dee,der ;
{
    double den ;
    
    den = pitagora(dee) ;
    m->sx = m->sy = (dee.x*der.x + dee.y*der.y)/den ;
    m->syx = (dee.x*der.y - dee.y*der.x) / den ;
    m->sxy = - m->syx ;
} /* end of ruota() */

static void rotscal(m,s,a)
matrix *m ;
double s, a ;
{
    double ca, sa ;
    ca = cos(a) ;
    sa = sin(a) ;
    m->sx = s*ca ;
    m->sxy = -s*sa ;
    m->syx = s*sa ;
    m->sy = s*ca ;
} /* end of rotscal */

static void trasla(m,ee,er)
matrix *m ;
point ee,er ;
{
    m->dx = er.x - m->sx*ee.x - m->sxy*ee.y ;
    m->dy = er.y - m->syx*ee.x - m->sy*ee.y ;
} /* end of trasla() */

static void sort_specs(p,n)
spec_item *p ;
int n ;
{
    spec_item tmp ;
    int i,j,found ;
    /* bubble sort della tabella secondo il 'type' crescente */
    /* sara` vergognosamente lento, ma avro` 2-3 elementi da sortare! */
    found = 1 ;
    for(i=0; found && (i<n-1); ++i)
	for(j=found=0; j<n-i-1; ++j)
	    if(p[j].type>p[j+1].type)
	    {
		tmp = p[j] ;
		p[j] = p[j+1] ;
		p[j+1] = tmp ;
		found = 1 ;
	    }
} /* end of sort_specs() */
    
point mult_mv(m,v)
matrix m ;
point v ;
{
    point ret ;
    ret.x = m.sx*v.x + m.sxy*v.y + m.dx ;
    ret.y = m.syx*v.x + m.sy*v.y + m.dy ;
    return ret ;
} /* end of mult_mv() */

matrix mult_mm(m1,m2)
matrix m1,m2 ;
{
    matrix ret ;
    ret.sx = m1.sx*m2.sx + m1.sxy*m2.syx ;
    ret.sxy = m1.sx*m2.sxy + m1.sxy*m2.sy ;
    ret.dx = m1.sx*m2.dx + m1.sxy*m2.dy + m1.dx ;
    ret.syx = m1.syx*m2.sx + m1.sy*m2.syx ;
    ret.sy = m1.syx*m2.sxy + m1.sy*m2.sy ;
    ret.dy = m1.syx*m2.dx + m1.sy*m2.dy + m1.dy ;
    return ret ;
} /* end of mult_mm() */

matrix calc_matrix( p, num )
spec_item *p ;
int num ;
{
    matrix ret ;
    
    sort_specs( p, num ) ;
    
/* EMPTY */
    if (num==0)
    {
	rotscal(&ret,1.0,0.0) ;
	ret.dx = ret.dy = 0.0 ;
    }
/* T1 */
    else if(num==1 && p[0].type==1)
    {
	rotscal(&ret,1.0,0.0) ;
	trasla(&ret,p[0].ee,p[0].er) ;
    }
/* T2 */
    else if(num==1 && p[0].type==2)
    {
	ruota(&ret,p[0].ee,p[0].er) ;
	ret.dx = ret.dy = 0.0 ;
    }
/* T3 */
    else if(num==1 && p[0].type==3)
    {
	rotscal(&ret,p[0].val,0.0) ;
	ret.dx = ret.dy = 0.0 ;
    }
/* T4 */
    else if(num==1 && p[0].type==4)
    {
	rotscal(&ret,1.0,TO_RAD(p[0].val) ) ;
	ret.dx = ret.dy = 0.0 ;
    }
/* T1 T1 */
    else if(num==2 && p[0].type==1 && p[1].type==1)
    {
	point dee, der ;
	dee = delta(p[1].ee,p[0].ee) ;
	der = delta(p[1].er,p[0].er) ;
	ruota(&ret,dee,der) ;
	trasla(&ret,p[0].ee,p[0].er) ;
    }
/* T1 T2 */
    else if(num==2 && p[0].type==1 && p[1].type==2)
    {
	ruota(&ret,p[1].ee,p[1].er) ;
	trasla(&ret,p[0].ee,p[0].er) ;
    }
/* T1 T3 */
    else if(num==2 && p[0].type==1 && p[1].type==3)
    {
	rotscal(&ret,p[1].val,0.0) ;
	trasla(&ret,p[0].ee,p[0].er) ;
    }
/* T1 T4 */
    else if(num==2 && p[0].type==1 && p[1].type==4)
    {
	rotscal(&ret,1.0,TO_RAD(p[1].val)) ;
	trasla(&ret,p[0].ee,p[0].er) ;
    }
/* T2 T2 */
    else if(num==2 && p[0].type==2 && p[1].type==2)
    {
	point dee, der ;
	dee = delta(p[1].ee,p[0].ee) ;
	der = delta(p[1].er,p[0].er) ;
	ruota(&ret,dee,der) ;
	ret.dx = ret.dy = 0 ;
    }
/* T1 T1 T4 */
    else if(num==3 && p[0].type==1 && p[1].type==1 && p[2].type==4)
    {
	point dee, der ;
	double sa,ca ;
	double ax,ay ;
	dee = delta(p[1].ee,p[0].ee) ;
	der = delta(p[1].er,p[0].er) ;
	sa = sin( TO_RAD(p[2].val) ) ;
	ca = cos( TO_RAD(p[2].val) ) ;
	ax = der.x / (dee.x*ca - dee.y*sa) ;
	ay = der.y / (dee.x*sa + dee.y*ca) ;
	
	ret.sx = ax*ca ;
	ret.sxy = -ax*sa ;
	ret.syx = ay*sa ;
	ret.sy = ay*ca ;
	
	trasla(&ret,p[0].ee,p[0].er) ;
    }
/* T1 T2 T4 */
    else if(num==3 && p[0].type==1 && p[1].type==2 && p[2].type==4)
    {
	double sa,ca ;
	double ax,ay ;
	sa = sin( TO_RAD(p[2].val) ) ;
	ca = cos( TO_RAD(p[2].val) ) ;
	ax = p[1].er.x / (p[1].ee.x*ca - p[1].ee.y*sa) ;
	ay = p[1].er.y / (p[1].ee.x*sa + p[1].ee.y*ca) ;
	
	ret.sx = ax*ca ;
	ret.sxy = -ax*sa ;
	ret.syx = ay*sa ;
	ret.sy = ay*ca ;
	
	trasla(&ret,p[0].ee,p[0].er) ;
    }
/* T1 T3 T4 */
/* T1 T3 T4 */
    else if(num==3 && p[0].type==1 && p[1].type==3 && p[2].type==4)
    {
	double sa,ca, a ;
	sa = sin(TO_RAD(p[2].val)) ;
	ca = cos(TO_RAD(p[2].val)) ;
	a = p[1].val ;
	
	ret.sx = a*ca ;    ret.sxy= -a*sa ;
	ret.syx= a*sa ;    ret.sy = a*ca ;
	
	trasla(&ret,p[0].ee,p[0].er) ;
    }
/* UNRECOGNIZED */
    else ERROR(E_unk_parseq,NULL) ;
    /*check_matrix(ret,p,num) ;*/
    return ret ;
} /* end of calc_matrix() */

static void check_matrix (m,p,n)
matrix m ;
spec_item *p ;
int n ;
{
    int i ;
    point t ;
    double ax, ay ;
    
    printf("[ %f  %f  %f\n  %f  %f  %f ]\n",
	m.sx,m.sxy,m.dx,m.syx,m.sy,m.dy ) ;
    for (i=0; i<n; ++i)
    {
	printf("T%d ",p[i].type) ;
	switch(p[i].type)
	{
	    case 1:
		t = mult_mv(m,p[i].ee) ;
		printf("[%f,%f] -> [%f,%f] = [%f,%f]\n",
		    p[i].ee.x,p[i].ee.y,p[i].er.x,p[i].er.y,t.x,t.y) ;
		break ;
	    case 2:
		t.x = m.sx*p[i].ee.x + m.sxy*p[i].ee.y  ;
		t.y = m.syx*p[i].ee.x + m.sy*p[i].ee.y  ;
		printf("[%f,%f] -> [%f,%f] = [%f,%f]\n",
		    p[i].ee.x,p[i].ee.y,p[i].er.x,p[i].er.y,t.x,t.y) ;
		break ;
	    case 3:
		ax = sqrt(m.sx*m.sx+m.sxy*m.sxy) ;
		ay = sqrt(m.syx*m.syx+m.sy*m.sy) ;
		printf("%f = (%f,%f)\n",p[i].val,ax,ay) ;
		break ;
	    case 4:
		ax = TO_DEG(atan2(-m.sxy,m.sx)) ;
		ay = TO_DEG(atan2(m.syx,m.sy)) ;
		printf("%f = (%f,%f)\n",p[i].val,ax,ay) ;
		break ;
	}
    }
    getchar() ;
} /* end of check_matrix() */
