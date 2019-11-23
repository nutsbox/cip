/*-------------------------------------------------------------------
*  hash.c:         hash implementation file
*
*  Description:    Contains hashing routines
*
*  Date Created:   17 October 2019
*
*  Last Modified:  17 October 2019
*
*  History:        - 23 Nov 2019 (some cleanup in the comments)
*                  - 17 Oct 2019 (Initial version)
*
*  By Nestor A. Jaba-an. Copyright (c) 2019.
*  Email: nestor@nutsbox.ph
*
*/
#include "include/hash.h"
#include "include/nutsbox.h"
#include "include/cip.h"


/*----------------------------------------------------------
*  Function:    crc32(bool is_file, char *in)
*
*  Description: This function generates a corresponding 
*               crc32 checksum from the input string or file
*
*  On Entry:    in = the string or file to checksum
*               is_file = true | false
*
*  Returns:     (char *) the checksum
*
*---------------------------------------------------------*/
char * crc32(bool is_file, char *in) {
	#define SIZE 4
	unsigned char * out = (unsigned char *) malloc(SIZE * sizeof(unsigned char));
	long len;

	if (is_file) in = readfile(in, &len);
	else len = strlen(in);

	crc32_state ctx;
	crc32_init(&ctx);                                      /* initialize */
	crc32_update(&ctx, (const unsigned char*) in, len);    /* update     */
	crc32_finish(&ctx, out, SIZE);                         /* finish     */

	return bytox(out, SIZE);
}


/*----------------------------------------------------------
*  Function:    adler32(bool is_file, char *in)
*
*  Description: This function generates a corresponding 
*               adler32 checksum from the input string or file
*
*  On Entry:    in = the string or file to checksum
*               is_file = true | false
*
*  Returns:     (char *) the checksum
*
*---------------------------------------------------------*/
char * adler32(bool is_file, char *in) {
	#define SIZE 4
	unsigned char * out = (unsigned char *) malloc(SIZE * sizeof(unsigned char));
	long len;

	if (is_file) in = readfile(in, &len);
	else len = strlen(in);

	adler32_state ctx;
	adler32_init(&ctx);                                    /* initialize */
	adler32_update(&ctx, (const unsigned char*)in, len);   /* update     */
	adler32_finish(&ctx, out, SIZE);                       /* finish     */

	return bytox(out, SIZE);
}


/*----------------------------------------------------------
*  Function:    hash(unsigned char *in, char *algo)
*
*  Description: This function generates a corresponding 
*               hash key from the input string or file
*
*  On Entry:    in = the string to hash
*               algo = the hashing algorithm
*               is_file = true | false
*               key = as required by hashing algorithm
*
*  Returns:     (unsigned char *) the hash key string
*
*---------------------------------------------------------*/
char * hash(char *in, char *algo, bool is_file, char *key)
{
	int idx, in_len, err;
	unsigned long len;
	static unsigned char out[MAXBLOCKSIZE];
	unsigned char *hash_ptr = out;

	if (strcmp(algo, "siphash") == 0) {
		if (! key) die(114, algo);
		if (strlen(key) != 16) die(115, "16");

		char *input = NULL;
		uint64_t keyhash = 0;
		long ilen = 0;

		if (! is_file) {
			ilen = strlen(in);

			/* validated against https://duzun.me/playground/hash#siphash= */
			keyhash = siphash24(in, ilen, key);
		}
		else { /* siphash a file */
			input = readfile( in, &ilen );
			/* printf("-->size of %s - %ld\n", in, ilen); */
			keyhash = siphash24(input, ilen, key);
		}
		sprintf((char *)hash_ptr, "%" PRIx64, keyhash);
		return (char *) hash_ptr;
	}

	/* Register algorithms before using them */
	register_all_ciphers();
	register_all_hashes();

	/* get the index of the hash */
	idx = find_hash(algo);
	if (idx == -1) die(108, algo);

	/* call appropriate hash */
	len = sizeof(out);
	in_len = strlen(in);
	if (! is_file) {
		if ((err = hash_memory(idx, (const unsigned char*) in, in_len, out, &len)) != CRYPT_OK) {
			die(109, (char *) error_to_string(err));
		}
	}
	else {
		if ((err = hash_file(idx, (const char *)in, out, &len)) != CRYPT_OK) {
			die(109, (char *) error_to_string(err));
		}
	}

	/* printf("%s\n", bytox(hash_ptr, hash_descriptor[idx].hashsize)); */
	return bytox(hash_ptr, hash_descriptor[idx].hashsize);
}


/*----------------------------------------------------------
*  Function:    shakehash(int bits, char *msg, int size)
*
*  Description: hash a string based on shake3
*
*  On Entry:    bits = hashing strength (128 or 256)
*               in = the string to hash
*               size = size of hash (bytes), e.g., see below
*                      16 bytes = 128 bits
*                      32 bytes = 256 bits
*                      64 bytes = 512 bits
*
*  Returns:     (unsigned char *) hash in bytes
*
*---------------------------------------------------------*/
unsigned char * shakehash(int bits, char *in, long size)
{
	if (size <= 0) return NULL;

	int err;
	hash_state state;

	static unsigned char out[MAXBLOCKSIZE];
	unsigned char *p = out;

	/* ensure bits are either 128 or 256 */
	bits -= 128;
	bits = (bits == 128) ? 256 : 128;

	if ((err = sha3_shake_init(&state, bits)) != CRYPT_OK) {
		die(118, (char *) error_to_string(err));
	}
	if ((err = sha3_shake_process(&state, (const unsigned char *)in, size)) != CRYPT_OK) {
		die(119, (char *) error_to_string(err));
	}


	if ((err = sha3_shake_done(&state, out, sizeof(out))) != CRYPT_OK) {
		die(120, (char *) error_to_string(err));
	}

	return out;
}


/*----------------------------------------------------------
*  Function:    getshake()
*
*  Description: a wrapper function to shakehash()
*
*  On Entry:    is_file = true if input is a file, otherwise false
*
*  Returns:     (unsigned char *) hash in hex
*
*---------------------------------------------------------*/
unsigned char * getshake(bool is_file, int bits, char *in, int outlen)
{
	unsigned char * out = NULL;

	if (is_file) { /* if it's a file, *in contains file name */
		long fsize;
		char * fdata = readfile(in, &fsize);
		out = shakehash(bits, fdata, fsize);
	}
	else out = shakehash(bits, in, strlen(in));

	return (unsigned char *) bytox(out, outlen);
}
