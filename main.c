/*-------------------------------------------------------------------
*  cip.c:			short for cipher
*
*  Description:		A small utility program to perform encryption,
*					decryption, and hash for string and files
*
*  Date Created:   14 October 2019
*
*  Last Modified:  23 October 2019
*
*  History:        - 14 Oct. 2019 (v0.50 Initial version)
*                  - 23 Oct. 2019 (v0.51 - completed inclusion of siphash)
*                  - 05 Nov. 2019 (v0.52 - Encoding and decoding of base64 added)
*
*  By Nestor A. Jaba-an. Copyright (c) 2019.
*  Email: nestor@nutsbox.ph
*
*/
#include <stdio.h>
#include "include/cip.h"
#include "include/hash.h"
#include "include/nutsbox.h"
#include "include/uuid4.h"
#include "include/base64.h"
#include "include/b64.h"
#include "include/cipher.h"


/*
*  Globals
*/
char *msg=NULL, *cipher=NULL, *fname=NULL;
char *algo=NULL, *key=NULL, *outfile=NULL;
int len=0;


int main(int argc, char *argv[]) 
{
	/* No command line argument(s) given */
	if (argc == 1) die(100, NULL);

	/* Make sure no invalid options are given */
	if ( ! isOptionValid(argc, argv) ) exit(EXIT_FAILURE);


	/*----------------------------------------------------------------------- 
	*   Perform task based on given option. Also, at this point. The given
	*   options are assumed to be valid
	*----------------------------------------------------------------------*/


	/* String to hash or encrypt */
	if (isOptionGiven(argv, argc, "-s")) {
		int pos = getParamPos(argv, argc, "-s");
		msg=getSubParam(argv, pos);
		if ( ! msg ) die(102, NULL);
		if (strcmp(left(msg,1),"-") == 0) die(103, msg);
	} /* At this point, we assume the text to hash or encrypt is already valid */


	/* File to hash or encrypt */
	if (isOptionGiven(argv, argc, "-f")) {
		int pos = getParamPos(argv, argc, "-f");
		fname=getSubParam(argv, pos);
		if (! fname ) die(104, NULL);
		if (! file_exist(fname)) die(111, fname);
		if (strcmp(left(fname,1),"-") == 0) die(105, fname);
	}


	/* -a option */
	if (isOptionGiven(argv, argc, "-a")) {
		int pos = getParamPos(argv, argc, "-a");
		algo=getSubParam(argv, pos);
		if ( ! algo ) die(112, NULL);
		if (strcmp(left(algo,1),"-") == 0) die(113, algo);
		if ( !isHashAlgoValid(algo) ) die(108, algo);
	}


	/* Key as required */
	if (isOptionGiven(argv, argc, "-k")) {
		int pos = getParamPos(argv, argc, "-k");
		key=getSubParam(argv, pos);
		if ( ! key ) die(112, NULL);
		if (strcmp(left(key,1),"-") == 0) die(113, key);
	}


	/* -len option as required */
	if (isOptionGiven(argv, argc, "-len")) {
		int pos = getParamPos(argv, argc, "-len");
		char *ls = getSubParam(argv, pos);
		if (! isnum(ls)) die(117, ls);
		len=atoi(ls);
	}


	/* -ts option */
	if (isOptionGiven(argv, argc, "-ts")) {
		
		if (! msg) die(106, NULL);

		int pos = getParamPos(argv, argc, "-ts");
		algo=getSubParam(argv, pos);
		if(algo) {
			if (strcmp(left(algo,1),"-") == 0) algo = NULL;
		}

		/* Check if algorithm is given. If not, default to sha3-256 */
		if ( ! algo ) algo = DEFAULT_HASH;

		/* 
		*  At this point, we assume the string to hash already valid
		*  we should now proceed to check if the given algorithm is valid
		*/
		if ( !isHashAlgoValid(algo) ) die(108, algo);

		/* is this a SHAKE algorithm? */
		if (strcmp(left(algo,6), "shake3") == 0) {
			if (strcmp(algo, "shake3-128") == 0) {
				if (len == 0) len = 16;
				die(EXIT_SUCCESS, (char *)getshake(false, 128, msg, len));
			}
			if (strcmp(algo, "shake3-256") == 0) {
				if (len == 0) len = 32;
				die(EXIT_SUCCESS, (char *)getshake(false, 256, msg, len));
			}
		} 

		if (strcmp(algo, "crc32") == 0) die(EXIT_SUCCESS, crc32(false, msg));
		if (strcmp(algo, "adler32") == 0) die(EXIT_SUCCESS, adler32(false, msg));

		/* Perform the non-SHAKE hash and exit */
		die(EXIT_SUCCESS, hash(msg, algo, false, key)); 
	}


	/* -tf option */
	if (isOptionGiven(argv, argc, "-tf")) {

		/* Ensure that file really exists and accessible */
		if (! fname) die(110, NULL);
		if (! file_exist(fname)) die(111, fname);

		int pos = getParamPos(argv, argc, "-tf");
		algo=getSubParam(argv, pos);
		if(algo) {
			if (strcmp(left(algo,1),"-") == 0) algo = NULL;
		}

		/* Check if algorithm is given. If not, default to sha3-256 */
		if ( ! algo ) algo = DEFAULT_HASH;

		/* 
		*  At this point, we assume the file to hash already valid
		*  we should now proceed to check if the given algorithm is valid
		*/
		if ( !isHashAlgoValid(algo) ) die(108, algo);

		/* is this a SHAKE algorithm? */
		if (strcmp(left(algo,6), "shake3") == 0) {
			if (strcmp(algo, "shake3-128") == 0) {
				if (len == 0) len = 16;
				die(EXIT_SUCCESS, (char *)getshake(true, 128, fname, len));
			}
			if (strcmp(algo, "shake3-256") == 0) {
				if (len == 0) len = 32;
				die(EXIT_SUCCESS, (char *)getshake(true, 256, fname, len));
			}
		} 

		if (strcmp(algo, "crc32") == 0) die(EXIT_SUCCESS, crc32(true, fname));
		if (strcmp(algo, "adler32") == 0) die(EXIT_SUCCESS, adler32(true, fname));

		/* Perform the non-SHAKE hash and exit */
		die(EXIT_SUCCESS, hash(fname, algo, true, key)); 
	}


	/* -u option */
	if (isOptionGiven(argv, argc, "-u") ) {

		if (msg) die(124, msg);
		if (fname) die(124, fname);

		char *buf = (char *) malloc( UUID4_LEN * sizeof(char));
		
		/* generate uuid4 randomly */
		uuid4_init();
		uuid4_generate(buf);
		die(EXIT_SUCCESS, buf);
	}


	/* -be option */
	if (isOptionGiven(argv, argc, "-be") ) {
		int pos = getParamPos(argv, argc, "-be");
		outfile=getSubParam(argv, pos);

		if (fname && outfile) die(EXIT_SUCCESS, base64(true, true, (unsigned char *)fname, outfile));
		else if (fname) die(EXIT_SUCCESS, base64(true, true, (unsigned char *)fname, NULL));

		if (msg && outfile) die(EXIT_SUCCESS, base64(false, true, (unsigned char *)msg, outfile));
		else if (msg) die(EXIT_SUCCESS, base64(false, true, (unsigned char *)msg, NULL));

		die(127, NULL);
	}


	/* -bd option */
	if (isOptionGiven(argv, argc, "-bd") ) {
		int pos = getParamPos(argv, argc, "-bd");
		outfile=getSubParam(argv, pos);

		if (fname && outfile) die(EXIT_SUCCESS, base64(true, false, (unsigned char *)fname, outfile));
		else if (fname) die(EXIT_SUCCESS, base64(true, false, (unsigned char *)fname, NULL));

		if (msg && outfile) die(EXIT_SUCCESS, base64(false, false, (unsigned char *)msg, outfile));
		else if (msg) die(EXIT_SUCCESS, base64(false, false, (unsigned char *)msg, NULL));

		die(127, NULL);
	}


	/* -us option */
	if (isOptionGiven(argv, argc, "-us") ) {
		if (! msg) die(122, NULL);
		/* generate uuid4 based on string */
		die(EXIT_SUCCESS, uuid4_gets(false, msg));
	}


	/* -uf option */
	if (isOptionGiven(argv, argc, "-uf") ) {
		if (! fname) die(123, NULL);
		/* generate uuid4 based on string */
		die(EXIT_SUCCESS, uuid4_gets(true, fname));
	}


	/* -c option */
	if (isOptionGiven(argv, argc, "-c")) {
		int pos = getParamPos(argv, argc, "-c");
		cipher=getSubParam(argv, pos);
		if ( ! cipher ) die(112, NULL);
		if (strcmp(left(cipher,1),"-") == 0) die(113, cipher);
	}


	/* -es | -ef option */
	if (isOptionGiven(argv, argc, "-es") || isOptionGiven(argv, argc, "-ef") ) {

		int pos = 0;
		if (isOptionGiven(argv, argc, "-ef")) {
			if (! fname) die(129, NULL);
			if (! file_exist(fname)) die(111, fname);
			pos = getParamPos(argv, argc, "-ef");
		}
		else {
			if (!msg) die(128, NULL);
			pos = getParamPos(argv, argc, "-es");
		}

		if (pos > 0) {
			outfile=getSubParam(argv, pos);
			if (outfile) {
				if (strcmp(left(outfile,1),"-") == 0) outfile = NULL;
			}
		}
		
		if (cipher) {
			if (strcmp(left(cipher,1),"-") == 0) cipher = NULL;
		}
		if ( ! cipher ) cipher = DEFAULT_CIPHER; 

		if (algo) {
			if (strcmp(left(algo,1),"-") == 0) algo = NULL;
		}
		if ( ! algo ) algo = DEFAULT_HASH;

		/* printf("OS: %s\n", get_platform_name()); */
		if (!key || strlen(key)==0) key = getkey();

		if (fname && outfile) die(EXIT_SUCCESS, krypt(true, true, fname, outfile, cipher, key, algo));
		else if (fname) die(EXIT_SUCCESS, krypt(true, true, fname, NULL, cipher, key, algo));
		
		if (msg && outfile) die(EXIT_SUCCESS, krypt(false, true, msg, outfile, cipher, key, algo));
		else if (msg) die(EXIT_SUCCESS, krypt(false, true, msg, NULL, cipher, key, algo));

		die(127, NULL);
	}


	/* -ds | -df option */
	if (isOptionGiven(argv, argc, "-ds") || isOptionGiven(argv, argc, "-df") ) {

		int pos = 0;
		if (isOptionGiven(argv, argc, "-df")) {
			if (! fname) die(132, NULL);
			if (! file_exist(fname)) die(111, fname);
			pos = getParamPos(argv, argc, "-df");
		}
		else {
			if (!msg) die(131, NULL);
			pos = getParamPos(argv, argc, "-ds");
		}

		if (pos > 0) {
			outfile=getSubParam(argv, pos);
			if (outfile) {
				if (strcmp(left(outfile,1),"-") == 0) outfile = NULL;
			}
		}
		
		if (cipher) {
			if (strcmp(left(cipher,1),"-") == 0) cipher = NULL;
		}
		if ( ! cipher ) cipher = DEFAULT_CIPHER; 

		if (algo) {
			if (strcmp(left(algo,1),"-") == 0) algo = NULL;
		}
		if ( ! algo ) algo = DEFAULT_HASH;

		if (!key || strlen(key)==0) key = getkey();

		if (fname && outfile) die(EXIT_SUCCESS, krypt(true, false, fname, outfile, cipher, key, algo));
		else if (fname) die(EXIT_SUCCESS, krypt(true, false, fname, NULL, cipher, key, algo));

		if (msg && outfile) die(EXIT_SUCCESS, krypt(false, false, msg, outfile, cipher, key, algo));
		else if (msg) die(EXIT_SUCCESS, krypt(false, false, msg, NULL, cipher, key, algo));

		die(127, NULL);
	}


	/* User is requesting for a version info */
	if (isOptionGiven(argv, argc, "-v") ) {
		version();
		exit(EXIT_SUCCESS);
	}


	/* User is requesting for help */
	if (isOptionGiven(argv, argc, "-h")) {
		help();
		exit(EXIT_SUCCESS);
	}


	/* Reaching this point means no ops */
	die(121, NULL);
}
