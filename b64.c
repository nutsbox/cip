/*-------------------------------------------------------------------
*  b64.c:          Source file for b64 encoding and decoding
*
*  Description:	   This is a library of general purpose functions
*                  used for whatever C/C++ programming endeavor
*
*  Date Created:   4 November 2019
*
*  Changelog:      - 11/4/2019 (initial working version)
*
*  Credits:        by John Walker (http://www.fourmilab.ch/)
*                  the base64 encoding and decoding engine
*                  is based on his BASE64 command line utility
*                  which is Public Domain.
*
*                  I converted to a form of library that I think
*                  is thread safe.
*
*  By Nestor A. Jaba-an. Public Domain.
*
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>


#ifdef _WIN32
	#define FORCE_BINARY_IO
	#include <io.h>
	#include <fcntl.h>
#endif

#define LINELEN 72
#define MAXINLINE 256


typedef unsigned char byte;

static int errcheck = true;
static char eol[] =
	#ifdef FORCE_BINARY_IO
	"\n"
	#else
	"\r\n"
	#endif
;


/*----------------------------------------------------------
*  Function:    static bool inbuf()
*
*  Description: Internal function used to read file into
*               a buffer.
*
*  On Entry:    fi - input file handle
*               iobuf - address of buffer to put bytes read
*               iolen - address-of-int to put length of bytes read
*               iocp - address-of-int (byte index)
*               ateof - a flag that indicates whether to read or
*                       not yet from a file
*
*  Returns:     false if eof, otherwise true
*
*---------------------------------------------------------*/
static bool inbuf(FILE *fi, byte *iobuf, int *iolen, int *iocp, bool *ateof)
{
	int fsize;

	if(*ateof){
		return false;
	}

	fsize = fread(iobuf, 1, MAXINLINE, fi);
	if(fsize <= 0) {
		if(ferror(fi)) exit(1);
		*ateof = true;
		return false;
	}

	*iolen = fsize;  /* update iolen with fsize */
	*iocp = 0;       /* reset iocp */

	return true;
}


/*----------------------------------------------------------
*  Function:    static int inchar()
*
*  Description: Reads and returns one char from buffer
*
*  On Entry:    fi - input file handle
*
*               iolen - address-of-int (current line length)
*               iocp - address-of-int (current byte index)
*               iobuf - address of buffer to read a byte from
*
*  Returns:     the character read (in int)
*
*---------------------------------------------------------*/
static int inchar(FILE *fi, int *iocp, int *iolen, byte *iobuf, bool *ateof)
{
	if(*iocp >= *iolen) {
		/* if chars are exhausted, read MAXINLINE chars again */
		/* on return iocp is reset, iolen contains actual bytes read */
		if(! inbuf(fi, iobuf, iolen, iocp, ateof)) return EOF;
	}

	return iobuf[(*iocp)++];
}


static void ochar(FILE *fo, int c, int *linelength)
{
	if(*linelength >= LINELEN) {
		if( fputs(eol, fo) == EOF ) exit(1);
		*linelength = 0;
	}
	if( putc( ((byte)c), fo) == EOF) exit(1);
	(*linelength)++;
}


void encode(FILE *fi, FILE *fo)
{
    /* if fi and fo are not valid, return */
    if (!fi) return;
    if (!fo) return;

	bool ateof = false;
	int linelength = 0;
	byte dtable[MAXINLINE];
	int i, hiteof = false;
	int iocp = MAXINLINE;      /* points current char in buffer */
	int iolen = 0;                         /* actual bytes read */
	byte * iobuf = (byte *) malloc(MAXINLINE); /* output buffer */

	/* start initialize dtable */
	for(i=0; i<9; i++){
		dtable[i] = 'A'+i;
		dtable[i+9] = 'J'+i;
		dtable[26+i] = 'a'+i;
		dtable[26+i+9] = 'j'+i;
	}
	for(i= 0;i<8;i++){
		dtable[i+18] = 'S'+i;
		dtable[26+i+18] = 's'+i;
	}
	for(i= 0;i<10;i++){
		dtable[52+i] = '0'+i;
	}
	dtable[62] = '+';
	dtable[63] = '/';
	/* end of initialization */

	while(!hiteof){
		byte igroup[3],ogroup[4];
		int c, n;

		igroup[0]= igroup[1]= igroup[2]= 0;

		for(n=0; n<3; n++) {
			c = inchar(fi, &iocp, &iolen, iobuf, &ateof);
			if(c == EOF){
				hiteof= true;
				break;
			}
			igroup[n] = (byte) c;
		}

		if(n > 0){
			ogroup[0] = dtable[ igroup[0] >> 2 ];
			ogroup[1] = dtable[ ((igroup[0] & 3) << 4) | (igroup[1] >> 4) ];
			ogroup[2] = dtable[ ((igroup[1] & 0xF) << 2) | (igroup[2] >> 6) ];
			ogroup[3] = dtable[ igroup[2] & 0x3F ];

			if(n < 3){
				ogroup[3] = '=';
				if(n < 2) ogroup[2] = '=';
			}

			for(i=0; i<4; i++) ochar(fo, ogroup[i], &linelength);
		}
	}
	if( fputs(eol, fo) == EOF ) exit(1);
}


static int insig(FILE *fi, int *iocp, int *iolen, byte *iobuf, bool *ateof)
{
	int c;

	while(true) {
		c = inchar(fi, iocp, iolen, iobuf, ateof);
		if(c == EOF || (c > ' ')) {
			return c;
		}
	}
}


void decode(FILE *fi, FILE *fo)
{
    /* if fi and fo are not valid, return */
    if (!fi) return;
    if (!fo) return;

    bool ateof = false;
    int i, iocp = MAXINLINE;
	int iolen = 0;                         /* actual bytes read */
	byte dtable[MAXINLINE];
	byte * iobuf = (byte *) malloc(MAXINLINE); /* output buffer */

	for(i=0; i<255; i++){
		dtable[i] = 0x80;
	}
	for(i='A'; i<='I'; i++){
		dtable[i] = 0+(i-'A');
	}
	for(i='J'; i<='R'; i++){
		dtable[i] = 9+(i-'J');
	}
	for(i='S'; i<='Z'; i++){
		dtable[i] = 18+(i-'S');
	}
	for(i= 'a';i<='i';i++){
		dtable[i]= 26+(i-'a');
	}
	for(i= 'j';i<='r';i++){
		dtable[i]= 35+(i-'j');
	}
	for(i= 's';i<='z';i++){
		dtable[i]= 44+(i-'s');
	}
	for(i= '0';i<='9';i++){
		dtable[i]= 52+(i-'0');
	}
	dtable['+']= 62;
	dtable['/']= 63;
	dtable['=']= 0;

	while(true) {
		byte a[4], b[4], o[3];

		for(i=0; i<4; i++) {
			int c= insig(fi, &iocp, &iolen, iobuf, &ateof);

			if(c==EOF){
				if(errcheck && (i> 0)){
					fprintf(stderr,"Input file incomplete.\n");
					exit(1);
				}
				return;
			}
			if(dtable[c]&0x80){
				if(errcheck){
					fprintf(stderr,"Illegal character '%c' in input file.\n",c);
					exit(1);
				}

				i--;
				continue;
			}
			a[i]= (byte)c;
			b[i]= (byte)dtable[c];
		}

		o[0]= (b[0]<<2)|(b[1]>>4);
		o[1]= (b[1]<<4)|(b[2]>>2);
		o[2]= (b[2]<<6)|b[3];
		i= a[2]=='='?1:(a[3]=='='?2:3);

		if(fwrite(o,i,1,fo)==EOF){
			exit(1);
		}

		if(i<3){
			return;
		}
	}
}


/*----------------------------------------------------------
*  Function:    char *s_encode(char *in)
*
*  Description: encodes a given string into b64
*
*  On Entry:    *in - NULL terminated string to convert
*
*  Returns:     (char *) b64 encoded NULL terminated string
*
*---------------------------------------------------------*/
char *s_encode(char *in)
{
    FILE *fi = tmpfile();
    FILE *fo = tmpfile();
    char *out;
    unsigned int i;

    if(fi == NULL || fo == NULL) {
        fprintf(stderr, "fatal error: unable to instantiate output stream\n");
        return NULL;
	}

    /* initialize input stream from given bytes, *in         */
    for (i=0; i < strlen(in); i++) fputc(in[i], fi);
    rewind(fi);              /* reset file pointer */

	encode(fi, fo);              /* perform actual b64 encoding */
	fclose(fi);                  /* close input stream */

	if(fseek(fo, 0, SEEK_END) != 0) return NULL;
	int outsize = ftell(fo);
	out = (char *) malloc(outsize + 1);    /* allocate mem for output buffer */

	/* copy encoding result to output buffer */
	/* minus the CR/LF/NULL character appended by encoder */
	rewind(fo);
	i=0;
	while (!feof(fo)) {
        int c = fgetc(fo);
        if (c != 10 && c != 13 && c != 0) out[i++] = c;
	}
	fclose(fo);
	out[i] = '\0';

	return out;
}


/*----------------------------------------------------------
*  Function:    char *s_decode(char *in)
*
*  Description: encodes a given string into b64
*
*  On Entry:    *in - NULL terminated string to decode
*
*  Returns:     (char *) decoded NULL terminated string
*
*---------------------------------------------------------*/
char *s_decode(char *in)
{
    FILE *fi = tmpfile();
    FILE *fo = tmpfile();
    char *out;
    unsigned int i;

    if(fi == NULL || fo == NULL) {
        fprintf(stderr, "fatal error: unable to instantiate output stream\n");
        return NULL;
	}

    /* initialize input stream from given bytes *in */
    /* minus the NULL terminator                     */
    for (i=0; i < strlen(in)-1; i++) fputc(in[i], fi);

    rewind(fi);                  /* reset file pointer */
	decode(fi, fo);              /* perform actual b64 encoding */
	fclose(fi);                  /* close input stream */

	if(fseek(fo, 0, SEEK_END) != 0) return NULL;
	int outsize = ftell(fo);
	out = (char *) malloc(outsize);    /* allocate mem for output buffer */

	/* copy encoding result to output buffer */
	rewind(fo);
	i=0;
	while (!feof(fo)) out[i++] = fgetc(fo);
	fclose(fo);

	out[i] = '\0';

	return out;
}


static bool is_existfile(char *filename)
{
	struct stat   buffer;
	return (stat (filename, &buffer) == 0);
}


void f_encode(char *infile, char *outfile)
{
    if (! is_existfile(infile) ) {
        fprintf(stderr, "fatal error: %s does not exist or is not accessible\n", infile);
        return;
    }

    FILE *fi = fopen(infile, "rb");
    FILE *fo = fopen(outfile, "wb");

    encode(fi, fo);

    fclose(fi);
	fclose(fo);
}


void f_decode(char *infile, char *outfile)
{
    if (! is_existfile(infile) ) {
        fprintf(stderr, "fatal error: %s does not exist or is not accessible\n", infile);
        return;
    }

    FILE *fi = fopen(infile, "rb");
    FILE *fo = fopen(outfile, "wb");

    decode(fi, fo);

    fclose(fi);
	fclose(fo);
}


/*----------------------------------------------------------
*  Function:    void sf_encode(char *in, char *outfile)
*
*  Description: encodes a given string into output file
*
*  On Entry:    *in - NULL terminated string to encode
*               *outfile - output file
*
*  Returns:     NONE
*---------------------------------------------------------*/
void sf_encode(char *in, char *outfile)
{
    FILE *fi = tmpfile();
    FILE *fo = fopen(outfile, "wb");
    unsigned int i;

    if(fi == NULL || fo == NULL) {
        fprintf(stderr, "fatal error: unable to instantiate i/o stream\n");
        exit(1);
	}

    /* initialize input stream from given bytes, *in         */
    for (i=0; i < strlen(in); i++) fputc(in[i], fi);
    rewind(fi);              /* reset file pointer */

	encode(fi, fo);              /* perform actual b64 encoding */

	fclose(fi);                  /* close input stream */
	fclose(fo);                  /* close output file */
}


/*----------------------------------------------------------
*  Function:    void sf_decode(char *in, char *outfile)
*
*  Description: encodes a given string into output file
*
*  On Entry:    *in - NULL terminated string to decode
*               *outfile - output file
*
*  Returns:     NONE
*---------------------------------------------------------*/
void sf_decode(char *in, char *outfile)
{
    FILE *fi = tmpfile();
    FILE *fo = fopen(outfile, "wb");
    unsigned int i;

    if(fi == NULL || fo == NULL) {
        fprintf(stderr, "fatal error: unable to instantiate i/o stream\n");
        exit(1);
	}

    /* initialize input stream from given bytes *in */
    /* minus the NULL terminator                     */
    for (i=0; i < strlen(in)-1; i++) fputc(in[i], fi);

    rewind(fi);			/* reset file pointer */

	decode(fi, fo);		/* perform actual b64 decoding */

	fclose(fi);			/* close input stream */
	fclose(fo);			/* close output file */
}
