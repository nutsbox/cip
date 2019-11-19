/*-------------------------------------------------------------------
*  nutsbox.h:      Header file for nutsbox.c
*
*  Description:	   This is a library of general purpose functions
*                  used for whatever C/C++ programming endeavor
*
*  Date Created:   31 October 2009
*
*  Last Modified:  29 August 2018
*
*  History:        - 8/29/2018 (Added nfgets and ngets function)
*
*  By Nestor A. Jaba-an. Copyright (C) 2009-2013.
*
*/

#ifndef NUTSBOX_H_INCLUDED
#define NUTSBOX_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdbool.h>


#define MAX_CHARS_PER_LINE 255
#define MAX_STR 2000

#define INITSIZE  112  /* power of 2 minus 16, helps malloc */
#define DELTASIZE (INITSIZE + 16)


/*----------------------------
*   String functions
*---------------------------*/
#ifdef __cplusplus
	extern "C" {
#endif

char * readfile( char *fname, long *flen );
char * bytox(unsigned char *bytes, int size);
bool isnum(char *snum);
uint64_t gettime_ns();
char *pullstr( char *s1, int pos, int length );
char *concat( char *s1, char *s2 );
char *ltrim( char *s );
char *rtrim( char *s );
char *trim( char *s );
char *ucase( char *s );
char *lcase( char *s );
char *pcase( char *s );
bool isthere( char *s, char *r );
char *intToString( int n );
char *left( char *s, int max );
char *wpl( char *s, int n );
int nfgets(char* *ln, FILE *f);
void nswap(void *a, void *b, size_t size);

#define ngets(ln) nfgets(ln, stdin)
#define acount(a) (sizeof(a)/sizeof(a[0]))

/* thanks http://stackoverflow.com/a/8198009 */
#define _base(x) ((x >= '0' && x <= '9') ? '0' : \
         (x >= 'a' && x <= 'f') ? 'a' - 10 : \
         (x >= 'A' && x <= 'F') ? 'A' - 10 : \
            '\255')
#define HEXOF(x) (x - _base(x))


#ifdef __cplusplus
	}
#endif

#endif // NUTSBOX_H_INCLUDED
