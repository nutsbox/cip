#ifndef BASE64_H_INCLUDED
#define BASE64_H_INCLUDED

#include <stdio.h>
#include <tomcrypt.h>


/*----------------------------
*   Hashing functions
*---------------------------*/
#ifdef __cplusplus
	extern "C" {
#endif

/* macros to calculate safe b64 sizes taken from the following   */
/* https://api-docs.iotivity.org/latest-c/base64_8h_source.html  */
/* from this discussion: https://stackoverflow.com/questions/13378815/base64-length-calculation */
#define B64ENCODE_OUT_SAFESIZE(x) ((((x) + 3 - 1)/3) * 4 + 1)
#define B64DECODE_OUT_SAFESIZE(x) (((x)*3)/4)


char * base64(bool is_file, bool is_encode, unsigned char *in, char *outfile);


#ifdef __cplusplus
	}
#endif

#endif // BASE64_H_INCLUDED
