/*-------------------------------------------------------------------
*  cip.c:			cip implementation file
*
*  Description:		Contains the meat of the functions
*
*  Date Created:   14 October 1019
*
*  Last Modified:  14 October 1019
*
*  History:        - 14 October 2019 (Initial version)
*
*  By Nestor A. Jaba-an. Copyright (c) 2019.
*  Email: nestor@nutsbox.ph
*
*/
#include "include/cip.h"


struct
{
    int err_code;
    const char* err_msg;
}errtab[] = ERR_MESSAGES;


/*----------------------------------------------------------
*  Function:    void bailout(int status, char * param)
*---------------------------------------------------------*/
void bailout(int status, char * param)
{
	FILE *o = (status == EXIT_SUCCESS) ? stdout : stderr;
    
    if(param) fprintf(o, "%s", param);

	exit(status);
}


/*----------------------------------------------------------
*  Function:    bool file_exist (char *filename)
*
*  Description: Checks whether file exists and accessible
*
*  On Entry:    filename = path and filename
*
*  Returns:		true if valid, otherwise false
*
*---------------------------------------------------------*/
bool file_exist (char *filename)
{
	struct stat buffer;
	return (stat (filename, &buffer) == 0);
}


/*----------------------------------------------------------
*  Function:    static void die(int status)
*---------------------------------------------------------*/
void die(int status, char * param)
{
	FILE *o = (status == EXIT_SUCCESS) ? stdout : stderr;

    if (status) {
    	int i = 0;
    	int err_size = acount(errtab);

    	for (i=0; i < err_size; i++) {
        	if (errtab[i].err_code == status) {
        		if (param)
            		fprintf(o, errtab[i].err_msg, param);
            	else
            		fprintf(o, "%s", errtab[i].err_msg);
            	fprintf(o, "\ntype \"cip -h\" for help\n");
           	 break;
        	}
    	}
    }
    else {
    	if(param) fprintf(o, "%s\n", param);
    }
   exit(status);
}


/*----------------------------------------------------------
*  Function:    bool isHashAlgoValid(char *algo)
*
*  Description: Checks whether a given hash algorithm is valid or not
*
*  Returns:		true if valid, otherwise false
*
*---------------------------------------------------------*/
bool isHashAlgoValid(char *algo)
{
	char *algos[] = HASH_ALGORITHMS;
	int n = acount(algos);
	int i;

	for (i=0; i < n; i++) {
		if (strcmp(algos[i], algo) == 0) return true;
	}
	return false;
}


/*----------------------------------------------------------
*  Function:    bool isCipherValid(char *cipher)
*
*  Description: Checks whether a given cipher is valid or not
*
*  Returns:		true if valid, otherwise false
*
*---------------------------------------------------------*/
bool isCipherValid(char *cipher)
{
	char *ciphers[] = CIPHERS;
	int n = acount(ciphers);
	int i;

	for (i=0; i < n; i++) {
		if (strcmp(ciphers[i], cipher) == 0) return true;
	}
	return false;
}


/*--------------------------------------------------------------------
*  Function:    char * getSubParam(char **args, int i)
*
*  Description: Gets the sub-parameter of corresponding option
*               based on its index
*
*  Parameteres: args = address command line argument (argv)
*               i = the index of the option (main param)
*
*  Returns:		the sub-param, otherwise NULL
*
*-------------------------------------------------------------------*/
char * getSubParam(char **args, int i)
{
	if ( args[i + 1] ) return args[i + 1];
	return NULL;
}


/*--------------------------------------------------------------------
*  Function:    int getParamPos(char **args, int n, char *option)
*
*  Description: Gets the position of option in the command line
*
*  Parameteres: args = address command line argument (argv)
*               n = count of argument (argc)
*               option = the option to check if given
*
*  Returns:		index of the option, otherwise 0
*
*-------------------------------------------------------------------*/
int getParamPos(char **args, int n, char *option)
{
	int i;

	/* we start at index 1 to skip the program name */
	for (i=1; i < n; i++) {
		if (strcmp(args[i], option) == 0) return i;
	}
	return 0;
}


/*----------------------------------------------------------
*  Function:    void help(void)
*
*  Description: Displays version number
*
*---------------------------------------------------------*/
void help(void)
{
	fprintf(stdout, "\n usage:	cip [OPTIONS [SUB-PARAM]...]\n");
	fprintf(stdout, " encrypts, decrypts and hashes key for a given string or file\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " -s   <string>\t\tthe string to encrypt, decrypt or to hash\n");
	fprintf(stdout, " -f   <filename>\tthe file to encrypt, decrypt or to hash\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " -t*  [algorithm]\twhere '*' should be replaced by 's' or 'f'\n");
	fprintf(stdout, "\t\t\tperforms hash on given string (-ts) or filename (-tf)\n");
	fprintf(stdout, "\t\t\tsupported algorithms: md2 md4 md5 sha1 sha224 sha256\n");
	fprintf(stdout, "\t\t\tsha384 sha512 sha512-224 sha512-256 sha3-224\n");
	fprintf(stdout, "\t\t\tsha3-256 (default) sha3-384 sha3-512 blake2b-160\n");
	fprintf(stdout, "\t\t\tblake2b-256 blake2b-384 blake2b-512 whirlpool tiger\n");
	fprintf(stderr, "\t\t\tblake2s-128 blake2s-160 blake2s-224 blake2s-256 rmd128\n");
	fprintf(stdout, "\t\t\trmd160 rmd256 rmd320 chc_hash siphash crc32 adler32\n");
	fprintf(stdout, "\t\t\tshake3-128 shake3-256\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " -len <length>\t\tcustom hash length (in bytes) for shake3-128/256.\n");
	fprintf(stdout, "\t\t\tdefaults: shake3-128 (16 bytes), shake3-256 (32 bytes)\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " -k   <key>\t\tkey as required by the following:\n");
	fprintf(stdout, "\t\t\t- siphash (i.e., SipHash-2-4): 16 characters\n");
	fprintf(stdout, "\t\t\t- ciphers (see below). if not given, user will be prompted\n");
	fprintf(stdout, "\t\t\tto provide a key--no specific number of characters\n");
	fprintf(stdout, "\t\t\trequired for cipher (think of this like a password)\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " -b*  [outfile]\t\twhere '*' should be replaced by 'e' to encode\n");
	fprintf(stderr, "\t\t\tor 'd' to decode in base64 the given string through '-s'\n");
	fprintf(stdout, "\t\t\tor file through '-f' options. if outfile is not specified,\n");
	fprintf(stdout, "\t\t\tresult will be dump to the screen (i.e., stdout)\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " -u[*]\t\t\twhere '*' may be replaced by 's' or 'f'\n");
	fprintf(stdout, "\t\t\tgenerates uuid4 randomly (-u) or from given string (-us)\n");
	fprintf(stderr, "\t\t\tor filename (-uf)\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " -e*  [outfile]\t\twhere '*' should be replaced by 's' or 'f'\n");
	fprintf(stdout, " -d*  [outfile]\t\tencrypts or decrypts a string (-es|-ds)\n");
	fprintf(stdout, "\t\t\tgiven through '-s' or file (-ef|-df) given through '-f'\n");
	fprintf(stdout, "\t\t\tand outputs to screen or to [outfile]. if '-c' is not\n");
	fprintf(stdout, "\t\t\tspecified, aes is the cipher used by default.\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " -c   <cipher>\t\tcipher to use for encryption/decryption.\n");
	fprintf(stdout, "\t\t\tciphers: aes (default) blowfish xtea rc2 rc5 rc6 twofish\n");
	fprintf(stdout, "\t\t\tsafer+ safer-k64 safer-sk64 safer-k128 safer-sk128\n");
	fprintf(stdout, "\t\t\tdes 3des cast5 noekeon skipjack anubis khazad\n");
	fprintf(stdout, "\t\t\tseed kasumi multi2 camellia\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " -a   <algorithm>\tset hashing algorithm to be used for.\n");
	fprintf(stdout, "\t\t\tencryption/decryption (see above -t* option for list)\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " -v\t\t\tversion info\n");
	fprintf(stdout, " -h\t\t\thelp (this page)\n");
}


/*--------------------------------------------------------------------
*  Function:    bool isOptionGiven(char **args, int n, char *option)
*
*  Description: Checks whether a given option is given or not
*
*  Parameteres: args = address command line argument (argv)
*               n = count of argument (argc)
*               option = the option to check if given
*
*  Returns:		true if given, otherwise false
*
*-------------------------------------------------------------------*/
bool isOptionGiven(char **args, int n, char *option)
{
	int i;

	/* we start at index 1 to skip the program name */
	for (i=1; i < n; i++) {
		if (strcmp(args[i], option) == 0) return true;
	}
	return false;
}


/*----------------------------------------------------------
*  Function:    bool isOptionValid(char *option)
*
*  Description: Checks whether a given option is valid or not
*
*  Returns:		true if valid, otherwise false
*
*---------------------------------------------------------*/
bool isOptionValid(int argc, char *argv[])
{
	char *options[] = CIP_OPTIONS;
	char *ls, *sp;
	int n = acount(options);
	int i, j, pos;

	for (i=1; i < argc; i++) {

		ls = argv[i];

		if ( strcmp(ls, "-s")==0  || strcmp(ls, "-f")==0   || 
			 strcmp(ls, "-ts")==0 || strcmp(ls, "-tf")==0  ||
			 strcmp(ls, "-k")==0  || strcmp(ls, "-len")==0 ||
			 strcmp(ls, "-u")==0  || strcmp(ls, "-us")==0  ||
			 strcmp(ls, "-uf")==0 || strcmp(ls, "-be")==0  || 
			 strcmp(ls, "-bd")==0 || strcmp(ls, "-es")==0  || 
			 strcmp(ls, "-ef")==0 || strcmp(ls, "-ds")==0  ||
			 strcmp(ls, "-df")==0 || strcmp(ls, "-c")==0   ||
			 strcmp(ls, "-a")==0) {
			sp = getSubParam(argv, i);
			if ( strcmp(ls, "-ts") != 0 && strcmp(ls, "-tf") != 0 &&
				 strcmp(ls, "-u") != 0  && strcmp(ls, "-us") != 0 &&
				 strcmp(ls, "-uf")!= 0  && strcmp(ls, "-be") != 0 &&
				 strcmp(ls, "-bd") != 0 && strcmp(ls, "-es") != 0 &&
				 strcmp(ls, "-ef") != 0 && strcmp(ls, "-ds") != 0 &&
				 strcmp(ls, "-df") != 0) {
				if ( ! sp ) die(112, ls);
			}
			if (sp && strcmp(ls, "-u") != 0  && strcmp(ls, "-us") != 0
				   && strcmp(ls, "-uf") != 0 && strcmp(ls, "-ts") != 0
				   && strcmp(ls, "-tf") != 0 && strcmp(ls, "-es") != 0
				   && strcmp(ls, "-ef") != 0 && strcmp(ls, "-ds") != 0
				   && strcmp(ls, "-df") != 0) {
				if (strcmp(left(sp,1),"-") == 0) die(113, sp);
				i++;
			}
		}
		else if ( strcmp(ls, "-v") != 0 && strcmp(ls, "-h") != 0 && strcmp(left(sp,1),"-") == 0 ) {
			die(101, ls);
		}
	}
	return true;
}


/*----------------------------------------------------------
*  Function:    void version(void)
*
*  Description: Displays version number
*
*---------------------------------------------------------*/
void version(void)
{
	fprintf(stdout, "\n cip %s - tool for encryption, decryption, and hashing of given string or file\n", APP_VERSION);
	fprintf(stdout, " by Nestor A. Jaba-an <nestor@nutsbox.ph>, 2019. Public Domain.\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " CREDITS:\n");
	fprintf(stdout, "   - Tom St Denis, LibTomCrypt v1.18.2 (PD), https://github.com/libtom/libtomcrypt\n");
	fprintf(stdout, "   - Tom St Denis, LibTomMath v1.2.0 (PD), https://github.com/libtom/libtommath\n");
	fprintf(stdout, "   - Daniel Richards, hashsum.c & ltcrypt.c (PD), <kyhwana@world-net.co.nz>\n");
	fprintf(stdout, "   - Majek, https://github.com/majek/csiphash/blob/master/csiphash.c (MIT Lic.)\n");
	fprintf(stdout, "   - Aumasson, J. P., & Bernstein, D. J., SipHash (PD)\n");
	fprintf(stdout, "   - Rxi, https://github.com/rxi/uuid4 (MIT Lic.)\n");
	fprintf(stdout, "   - John Walker, base64-1.5 (PD), http://www.fourmilab.ch/webtools/base64/index.html\n");
	fprintf(stdout, "\n");
}
