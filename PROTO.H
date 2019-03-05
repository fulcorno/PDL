/*
 * proto.h
 * definisce delle Macro che cercano di mascherare la differenza tra
 * compilatori ANSI e compilatori UNIX
 */

#ifndef PROTO

#if __STDC__ || __MSDOS__
 /* Il compilatore e` ANSI-compatibile */
#   define PROTO(s) s		/* prototypes accepted */
#else
 /* Compilatore UNIX-like */
#   define PROTO(s) ()		/* kill prototypes */
#endif				/* __STDC__ */

#endif				/* PROTO */

/*
 * Definisci dei nomi piu` brevi per i tipi "unsigned"
 */

#ifndef _UNSIGNED_NAMES_
typedef unsigned long ulong;
typedef unsigned char uchar;

/*    typedef unsigned int  uint  ;*/
#   define _UNSIGNED_NAMES_ 1
#endif				/* _UNSIGNED_NAMES_ */
