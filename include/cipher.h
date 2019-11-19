#ifndef CIPHPER_H_INCLUDED
#define CIPHPER_H_INCLUDED

#include <tomcrypt.h>
#include <stdbool.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
	#define PLATFORM_NAME "windows" // Windows
	#include <conio.h>
#elif defined(__linux__)
    #define PLATFORM_NAME "linux"
	#include <unistd.h>
#elif defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
	#include <sys/param.h>
	#if defined(BSD)
		#define PLATFORM_NAME "bsd"
	#endif
	#include <unistd.h>
#elif defined(__APPLE__) && defined(__MACH__)
	#define PLATFORM_NAME "apple"
	#include <unistd.h>
	#include <pwd.h>
#else
	#define PLATFORM_NAME "non_win"
	#include <unistd.h>
#endif

#include "cip.h"
#include "hash.h"
#include "nutsbox.h"


/*---------------------------------------
*   Encryption / Decryption functions
*--------------------------------------*/
#ifdef __cplusplus
	extern "C" {
#endif

/* implementatin is in other.c */


/* implementations is in cipher.c */
char * krypt (bool is_file, bool is_encrypt, char *in, char *out, char *cipher, char *inkey, char *algo);
#if defined (_WIN32) || defined(_WIN64)
char * readkey(char * prompt);
#endif
char * getkey();
unsigned char * gethash(char *algo, char *in, unsigned long *outlen);
int ctob(char * chars, unsigned char * bytes);
int ctobx(char * chars, unsigned char * bytes);
int ctod(char c);
char * get_platform_name();


#ifdef __cplusplus
	}
#endif

#endif // CIPHPER_H_INCLUDED
