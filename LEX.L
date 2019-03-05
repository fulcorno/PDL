%{
   /*
    * PDL-LEX
    * Scanner per il linguaggio PDL
    * Verra` incluso in gram.c
    */
    /*
    int yylook PROTO ((void)) ;
    int yyinput PROTO ((void)) ;
    int yyoutput PROTO ((int c)) ;
    int yyunput PROTO ((int c)) ;
    int yymore PROTO ((void)) ;
    */

    #undef yywrap
    /* Calmiamo il compilatore: */
    /*
#pragma warn -rch
    */

    /* WARNING: si assume che gli statement:
    	yyleng -= x ; yymore() ;
    facciano realmente scartare gli ultimi 'x' caratteri da yytext, a cui
    verra appeso il restante testo match-ato.
    Se cosi` non fosse, si dovra` usare yyless(), assente su PC
    */

%}

DIGIT [0-9]
LETTER [a-zA-Z_$]
IDENTIFIER {LETTER}({DIGIT}|{LETTER})*
OPERATOR [+\-*/()]
SEPARATOR [\[\],.=;]
EXPON ([eE][-+]?{DIGIT}+)

%start in_string in_comment

%%


"{"			{ BEGIN in_comment ; }
<in_comment>"\n" 	{ n_lines++ ; }
<in_comment>[^}\n]+ 	{}
<in_comment>"}"		{ BEGIN 0 ; }

<in_string>[^"\n\\]+	{ yymore() ; }
<in_string>\\\n		{ yyleng -= 2 ; yymore() ; n_lines++ ; }
<in_string>\\\"		{ yyleng-- ; yytext[yyleng-1] = '\"' ; yymore() ; }
<in_string>\\n		{ yyleng-- ; yytext[yyleng-1] = '\n' ; yymore() ; }
<in_string>\\r		{ yyleng-- ; yytext[yyleng-1] = '\r' ; yymore() ; }
<in_string>\\b		{ yyleng-- ; yytext[yyleng-1] = '\b' ; yymore() ; }
<in_string>\\t		{ yyleng-- ; yytext[yyleng-1] = '\t' ; yymore() ; }
<in_string>\\\\		{ yyleng-- ; yytext[yyleng-1] = '\\' ; yymore() ; }
<in_string>\n		{ ERROR(E_str_eol,NULL) ; }
<in_string>\\.		{ ERROR(E_str_esc,&yytext[yyleng-2]) ; }
<in_string>\"		{
			if(yyleng>MAX_ID_LEN+1) ERROR(E_str_long,yytext) ;
			yytext[yyleng-1] = '\0' ;
			yylval = new_str( yytext ) ;
			BEGIN 0 ;
			return string ;
			}
\"			{ BEGIN in_string ; }

"define"	{ return Define; }
"end"		{ return End; }
"angle"		{ return Angle; }
"length"	{ return Length; }
"versor"	{ return Versor; }
"to"		{ return To; }
"scale"		{ return Scale; }
"rotate"	{ return Rotate; }

"default"	{ return Default; }
"defval"	{ return Defval; }

"vector"	{ return Vector; }
"scalar"	{ return Scalar; }

"dash"		{ return Dash ; }
"fill"		{ return Fill ; }

{OPERATOR}|{SEPARATOR}	{ return yytext[0]; }

{IDENTIFIER} 	{
		    if(yyleng>MAX_ID_LEN) ERROR(E_id_long,yytext) ;
		    yylval = new_str(yytext) ;
		    return id ;
		}

{DIGIT}+			|
{DIGIT}+{EXPON}			|
{DIGIT}*"."{DIGIT}+{EXPON}?	|
{DIGIT}+"."{DIGIT}*{EXPON}?	{
				    yylval = new_num(atof(yytext)) ;
				    return num;
				}

[ \t]		;

\n		{ n_lines++ ; }

"#"	{ smt_dump() ; }

.	{ ERROR(E_inv_char,yytext) ; }

%%

/*
#pragma warn .rch
*/
