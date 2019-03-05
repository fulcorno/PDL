/*
 * ERR.h
 * Gestione degli errori
 * Definizione macro che non devono far ricompilare alla loro variazione
 */
 
#define RELEASE "1.2 22-10-1991"

enum err_codes {
/* special codes - implementor directed */
    E_none,
    E_nyi,		/* Not Yet Implemented */
    E_internal,		/* Internal Error */
/* user errors */
    E_unk_sw,		/* unknown command line switch */
    E_udim_syn,		/* wrong syntax for picture dimension (#) */
    E_file_nf,		/* input file not found */
    E_creat,		/* can''t create file */
/* lexical errors */
    E_id_long,		/* id too long */
    E_str_long, 	/* str too long */
    E_str_eol,		/* string across line boundary */
    E_str_esc,		/* bad escape sequence */
    E_inv_char,		/* invalid char in input */
/* syntax errors */
    E_lowlevel,		/* Low-Level error (from yacc) */
/* type checking */
    E_inv_t,		/* invalid type */
    E_inv_t_op,		/* invalid types for operator # */
/* scoping rules */
    E_notdef,		/* variable # not defined */
    E_dupl_sym,		/* duplicate symbol definition for # */
    E_not_init_var,	/* variable # not initialized */
/* compiler overflow */
    E_memory,		/* generic out of memory */
    E_handle_full,	/* out of handles */
    E_down_full,	/* out of down nodes */
/* semantic errors */
    E_decl_msm,		/* declaration 'id' mismatch */
    E_undef_loc,	/* undefined handle location */
    E_defval_dup,	/* too many 'defval' */
    E_notdef_obj,	/* object not defined */
    E_notval_obj,	/* object not valid */
    E_notval_ref,	/* object not valid */
    E_nonex_handle,	/* nonexistant handle */
    E_nonex_defval,	/* nonexistant defval */
    E_many_param,	/* too many parameters */
    E_many_def,		/* too many default requests */
    E_dupl_opt,		/* duplicate option # specification */
    E_inv_opt,		/* invalid value for option # */
/* graphic capabilities errors */
    E_unk_parseq,	/* unknown parameter specification */
    E_nested,		/* too deep figure nesting */
/* hardware errors */
    E_inv_device,	/* device # not supported */
    E_low_device,	/* device # not advanced enough */
    E_init_dev,		/* can't initialize device */
    E_miss_device,	/* missing device specification */
};

#ifdef MAIN_MODULE
static char *messages[] = {
	"NO ERROR - %s",
	"%s NOT YET implemented",
	"INTERNAL ERROR %s",
	
	"unknown switch \"%s\"",
	"wrong syntax for picture dimension \"#\"",
	"source file \"%s\" not found",
	"unable to create file \"%s\"",
	
	"identifier '%s' too long",
	"string \"%s\" too long",
	"newline encountered within string",
	"bad string escape sequence (%s)",
	"illegal character in source",
	
	"YACC says \"%s\"",
	
	"invalid type variable '%s'",
	"invalid types for operator %s",
	
	"variable '%s' not defined",
	"duplicate symbol definition for '%s'",
	"reference to uninitialized variable '%s'",
	
	"out of memory",
	"out of handle nodes space",
	"out of down nodes space",
	
	"'end' mismatches 'define %s'",
	"undefined location for handle '%s'",
	"duplicate 'defval' handle '%s'",
	"object '%s' not defined",
	"object '%s' not valid",
	"reference '%s' not valid",
	"missing handle '%s'",
	"missing 'defval'",
	"too many parameters",
	"object '%s' hasn't so many default's",
	"duplicte specification for option '%s'",
	"invalid specification for option '%s'",
	
	"can't compute transformation (sorry)",
	"too deeply nested drawing",
	
	"device '%s' not supported (sorry)",
	"device '%s' missing enough capabilities",
	"unable to initialize device: %s",
	"missing device specification",
} ;
#endif

