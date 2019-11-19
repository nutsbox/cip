#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include <stdio.h>
#include <tomcrypt.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/time.h>


/*----------------------------
*   Hashing functions
*---------------------------*/
#ifdef __cplusplus
	extern "C" {
#endif

/* implementatin is in csiphash.c */
uint64_t siphash24(const void *src, unsigned long src_sz, const char key[16]);

/* implementation is in hash.c */
char * crc32(bool is_file, char *in);
char * adler32(bool is_file, char *in);
char * hash(char *in, char *algo, bool is_file, char *key);
unsigned char * hash_md5(unsigned char *msg);
unsigned char * shakehash(int bits, char *in, long size);
unsigned char * getshake(bool is_file, int bits, char *in, int outlen);


#ifdef __cplusplus
	}
#endif

#endif // HASH_H_INCLUDED
