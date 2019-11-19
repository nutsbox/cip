/*-------------------------------------------------------------------
*  hash.c:         base_xx (e.g., base 64) implementation file
*
*  Date Created:   28 October 2019
*
*  Last Modified:  28 October 2019
*
*  Changelog:      - 28 October 2019 (Initial version)
*
*  By Nestor A. Jaba-an. Copyright (c) 2019.
*  Email: nestor@nutsbox.ph
*
*/
#include "include/nutsbox.h"
#include "include/cip.h"
#include "include/b64.h"
#include "include/base64.h"


unsigned long getlen(unsigned char *in)
{
	unsigned long outlen = 0;
	while(in[outlen] != '\0') outlen++;
	return outlen;
}

char * strip_crlf(unsigned char *in)
{
	unsigned long outlen = strlen((char *)in), i=0;
	char *out = (char *) malloc(outlen);

	while(*in != '\0')
	{
		int c = *in;
		if (c != 10 && c != 13) out[i++] = *in;
		in++;
	}

	out[i] = '\0';

	return out;
}


/*----------------------------------------------------------
*  Function:    base64(boolean )
*
*  Description: a wrapper function to libtomcrypt base64 functions
*
*  On Entry:    is_file = true if input is a file, otherwise false
*               is_encode = true if encoding, false if decoding
*               in = input data or string
*
*  Returns:     (unsigned char *) the encoded or decoded version
*
*---------------------------------------------------------*/
char * base64(bool is_file, bool is_encode, unsigned char *in, char *outfile)
{
	unsigned char * out = NULL;
	unsigned long outlen = 0;
	int err;

	if (is_file) { /* if it's a file, *in contains file name */
		if (is_encode) {
			if (outfile) { /* encode from file to file */
				f_encode((char *)in, outfile);
				out = (unsigned char *) concat(concat((char *)in, " -> "), outfile);
			}
			else { /* encode from file and return output */
				FILE *fi = fopen((const char *)in, "rb");
				FILE *fo = stdout;
				encode(fi, fo);
				fclose(fi);
				fclose(fo);
				return NULL;
			}
		}
		else { /* decode */
			if (outfile) { /* decode from file to file */
				f_decode((char *)in, outfile);
				out = (unsigned char *) concat(concat((char *)in, " -> "), outfile);
			}
			else { /* decode from file and return output */
				FILE *fi = fopen((const char *) in, "rb");
				FILE *fo = stdout;
				decode(fi, fo);
				fclose(fi);
				fclose(fo);
				return NULL;
			}
		}
	}
	else { /* we're encoding|decoding string */
		if (is_encode) {
			if (outfile) { /* encode from string to file */
				sf_encode((char *)in, outfile);
				out = (unsigned char *) concat(concat((char *) in, " encoded -> "), outfile);
			}
			else { /* encode from string to string */
				outlen = B64ENCODE_OUT_SAFESIZE( getlen(in) );
				out = (unsigned char *) malloc( outlen );
				if (base64_encode(in, getlen(in), out, &outlen) != CRYPT_OK) die(125, (char *) in);
			}
		}
		else { /* decode */
			if (outfile) { /* decode from string to file */
				sf_decode((char *) in, outfile);
				out = (unsigned char *) concat(concat((char *) in, " decoded -> "), outfile);
			}
			else { /* decode from string to string */ 
				outlen = B64DECODE_OUT_SAFESIZE(getlen(in));
				out = (unsigned char *) malloc( outlen );
				if (base64_strict_decode(in, getlen(in), out, &outlen) != CRYPT_OK) die(126, (char *) in);
			}
		}
	}

	return (char *) out;
}
