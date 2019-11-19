#ifndef CIP_H_INCLUDED
#define CIP_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "nutsbox.h"


/*
*  Globals
*/
#define APP_VERSION "v0.53"
#define DEFAULT_HASH "sha3-256"
#define DEFAULT_CIPHER "aes"
#define MULTI2_KEYSIZE 40

#define CIP_OPTIONS {"-v", "-h", "-s", "-f", "-ts", "-tf", "-u", "-len", "-us", "-uf", \
                     "-be", "-bd", "-es", "-ef", "-ds", "-df", "-c", "-a"}

#define HASH_ALGORITHMS { "md2", "md4", "md5", "sha1", "sha224", "sha256", "sha384", \
			"sha512", "sha512-224", "sha512-256", "sha3-224", "sha3-256", "sha3-384", "sha3-512", \
			"whirlpool", "blake2b-160", "blake2b-256", "blake2b-384", "blake2b-512", \
			"tiger", "blake2s-128", "blake2s-160", "blake2s-224", "blake2s-256", \
			"rmd128", "rmd160", "rmd256", "rmd320", "chc_hash", "siphash", \
            "shake3-128", "shake3-256", "crc32", "adler32" }

#define CIPHERS { "aes", "blowfish", "xtea", "rc2", "rc5", "rc6", \
            "twofish", "safer-k64", "safer-sk64", "safer-k128", "safer-sk128", \
            "safer+", "des", "3des", "cast5", "noekeon", "skipjack", "anubis", \
            "khazad", "seed", "kasumi", "multi2", "camellia"}

#define ERR_MESSAGES {  \
    		{100, "option(s) required."}, \
    		{101, "invalid option %s"}, \
    		{102, "missing sub-param required by -s"}, \
    		{103, "\"%s\" cannot be a sub-param for -s"}, \
    		{104, "missing sub-param required by -f"}, \
    		{105, "\"%s\" cannot be a sub-param for -f"}, \
    		{106, "-ts requires <string> to hash"}, \
    		{107, "\"%s\" cannot be a sub-param for -ts"}, \
    		{108, "invalid hashing algorithm: %s."}, \
    		{109, "error hashing data: %s"}, \
    		{110, "-tf requires <filename> to hash"}, \
    		{111, "\"%s\" does not exist or is not accessible"}, \
    		{112, "missing sub-param required for %s"}, \
    		{113, "invalid sub-param: %s"}, \
    		{114, "%s requires a key"}, \
    		{115, "key length must be %s characters"}, \
            {116, "unable to open %s"}, \
            {117, "param given to -len is not a number: %s"}, \
            {118, "could not inititialize SHAKE algorithm (%s)"}, \
            {119, "could not process SHAKE algorithm (%s)"}, \
            {120, "could not finish SHAKE algorithm (%s)"}, \
            {121, "no given operation(s) to perform"}, \
            {122, "-us requires <string> to generate uuid4"}, \
            {123, "-uf requires <filename> to generate uuid4"}, \
            {124, "random uuid4 (-u) does not require \"%s\""}, \
            {125, "base64 encoding error encountered for %s"}, \
            {126, "base64 decoding error encountered for %s"}, \
            {127, "no string or file given to perform an operation with"}, \
            {128, "-es requires <string> to encrypt"}, \
            {129, "-ef requires <filename> to encrypt"}, \
            {130, "keys do not match"}, \
            {131, "-ds requires <string> to encrypt"}, \
            {132, "-df requires <filename> to encrypt"}, \
            {133, "file i/o error encountered: %s"}, \
            {134, "cipher init failed or invalid cipher: %s"}, \
            {135, "invalid key size"}, \
            {136, "%s algorithm is not compatible with this cipher, try other combination"}, \
            {137, "unable to setup PRNG: %s"}, \
            {138, "unable to generate required random number for IV"}, \
            {139, "error writing IV"}, \
            {140, "ctr_start error: %s"}, \
            {141, "ctr_encrypt error: %s"}, \
            {142, "error writing output file: %s"}, \
            {143, "error reading IV from input: %s"}, \
            {144, "ctr_decrypt error: %s"}, \
            {0, NULL} }


/*
*  Function declarations
*/

void bailout(int status, char * param);
bool file_exist (char *filename);
void die(int status, char * param);
bool isHashAlgoValid(char *algo);
char * getSubParam(char **args, int i);
int getParamPos(char **args, int n, char *option);
void help(void);
bool isOptionGiven(char **args, int n, char *option);
bool isOptionValid(int argc, char *argv[]);
void version(void);


#endif // CIP_H_INCLUDED
