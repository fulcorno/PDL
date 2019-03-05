/*
 * PDL - Picture Description Language
 * modulo principale del traduttore
 */

#define MAIN_MODULE

#include "types.h"

extern FILE *yyin, *yyout ;
extern int yydebug ;
static int arc, n_arg ;
static char **arv ;

int main ( argc, argv )
int argc ;
char ** argv ;
{
    char *out ;
    int i ;

    /* inizializzazione di variabili globali */
    n_lines = 0 ;

    yyin = stdin ;
    yyout = stderr ;

    /* lettura della linea di comando */
    /* defaults: */
    out = getenv("DEVICE") ;
    q_flg = 0 ;
    i=1 ;
    while(i<argc && argv[i][0]=='-')
    {
        switch(argv[i][1])
        {
            case 'o': out = argv[++i] ; break ;
	    case 'q': q_flg=1 ; break ;
	    case 'd': d_flg=1 ;
	              if(1!=sscanf(argv[++i],"%lf",&user_dim.x))
		         ERROR(E_udim_syn,argv[i]) ;
	              if(1!=sscanf(argv[++i],"%lf",&user_dim.y))
		         ERROR(E_udim_syn,argv[i]) ;
		     break ;
            default: ERROR(E_unk_sw,argv[i]) ;
        }
        ++i ;
    }
    
    if(!q_flg)
    {
	/* Messaggio iniziale */
	printf("<PDL> the device-independent Picture Description Language\n") ;
	printf("\tAuthor:  Fulvio Corno\n") ;
	printf("\tRelease: %s\n", RELEASE) ;
	printf("\tSupported devices:") ; device_info() ; printf("\n") ;
    }

    if (i<argc) yyin = fopen(argv[i],"r") ;
    if(yyin) {
        arc = i+1 ;
        n_arg = argc ;
        arv = argv ;
        if(!q_flg) printf("builtin: ") ;
        smtinit() ;
        create_builtin() ;
	if(!q_flg)
	    if(i<argc) printf("\n%s: ",argv[i]) ;
	    else printf("\nstdin: ") ;
        n_lines = 1 ;
        yyparse() ;
        n_lines = -1 ;
        draw_it(out) ;
	n_lines = 0 ;
        smt_stat() ;
    } else ERROR(E_file_nf,argv[i]) ;
    return 0 ;
}

void ERROR ( reason, info )
enum err_codes reason ;
char *info ;
{
    char msg[255] ;

    if((reason < 0) || (reason>=(sizeof(messages)/sizeof(char*))))
        sprintf(msg,"UNKNOWN CODE %d - %s",reason,info) ;
    else sprintf(msg,messages[reason],info) ;
    if(n_lines<0) printf("\nError in execution: %s\n",msg) ;
    else if(n_lines==0) printf("\nError in setup: %s\n",msg) ;
    else if(yyin==stdin) printf("\nError in line %d: %s\n",n_lines,msg) ;
    else printf("\nError in \"%s\" %d: %s\n",arv[arc-1],n_lines,msg) ;
    exit(1) ;
} /* end of ERROR() */

int yywrap()
{
    n_lines=0 ;
    if(yyin != stdin) fclose(yyin) ;
    if(arc<n_arg)
    {
        yyin = fopen(arv[arc],"r") ;
        if(!q_flg) printf("\n%s: ",arv[arc]) ;
        if(!yyin) ERROR(E_file_nf,arv[arc]) ;
        arc ++ ;
        n_lines = 1 ;
        return 0 ;
    }
    return 1;
}

void yyerror(s)
char *s ;
{
    ERROR(E_lowlevel,s) ;
}
