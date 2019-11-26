/*-------------------------------------------------------------------
*  nutsbox.c:      Source file for libnutsbox.a
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
#include "include/nutsbox.h"


/*----------------------------------------------------------
*  Function:    char * readfile( char *fname, long *flen )
*
*  Description: This function reads a file and returns 
*               its contents and length
*
*  On Entry:    fname = file path
*               flen = the address of long int where to
*                      content length
*
*  Returns:     (unsigned char *) the content
*
*---------------------------------------------------------*/
char * readfile( char *fname, long *flen )
{
    char * content;
    FILE* fileptr = fopen(fname, "rb");
    
    if (!fileptr) {
        fprintf(stderr, "fatal error: unable to open %s\n", fname);
	return NULL;
    }

    fseek(fileptr, 0, SEEK_END);
    *flen = ftell(fileptr);

    rewind(fileptr);
    content = (char*) malloc((*flen) * sizeof(char));
    fread(content, (size_t) *flen, 1, fileptr);
    fclose(fileptr);

    return content;
}


/*----------------------------------------------------------
*  Function:    unsigned char * bytox(unsigned char *bytes, int size)
*
*  Description: btos (byte-to-string)
*               Converts a NULL terminated series of bytes
*               into its character equivalent.
*
*  On Entry:    bytes = string of bytes
*
*  Returns:     (unsigned char *) hex equivalent
*
*---------------------------------------------------------*/
char * bytox(unsigned char *bytes, int size)
{
    int i;
    unsigned char * str = (unsigned char *) malloc( size * 2 + 1 );
    unsigned char * p = &str[0];
    for (i=0; i<size; i++) {
        p += sprintf((char *)p, "%02x", bytes[i]);
    }
    return (char *)str;     /* sprintf auto-null terminate its result */
}


/*----------------------------------------------------------
*  Function:    bool isnum(char *snum)
*
*  Description: Checks if a given string is a qualified number
*
*  Returns:     true if number, otherwise false
*
*---------------------------------------------------------*/
bool isnum(char *snum)
{
    int i; 
    for (i=0; isdigit(snum[i]); i++);

    if (i==strlen(snum)) return true;
    return false;
}


/*----------------------------------------------------------
*  Function:    gettime_ns()
*
*  Description: For high-precision timing calculation
*
*  Usage:       uint64_t t0, t1;
*               #define REPEATS 1
*               ... <perform processing> ...
*               (t1-t0)/1000000. (microseconds calculation)
*               (t1-t0)/(REPEATS*64.) (nanosecods)
*  Example:
*               printf("\n%i tests passed in %.3fms, %.0fns per test\n", 
*                       REPEATS*64, (t1-t0)/1000000., 
*                       (t1-t0)/(REPEATS*64.));
*
*---------------------------------------------------------*/
uint64_t gettime_ns() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000000ULL + tv.tv_usec * 1000ULL;
}


/*----------------------------------------------------------
*  Function:    pullstr()
*
*  Description: This extracts a string from the given string.
*
*  On Entry:    s1 = The source string
*               pos = The position in the string to begin
*                     extracting from (first character starts at 1)
*               length = The number of characters to extract
*
*  Returns:     (char *) pointer to the extracted string
*               NULL if there's an error.
*
*---------------------------------------------------------*/
char *pullstr( char *s1, int pos, int length ) {

    char *p = (char *) malloc( length + 1);

    /* Save the original position */
    char *r = p;

    if (pos >= strlen(s1)) {
        return NULL;
    }

    if (length >= strlen(s1)) {
        return NULL;
    }

    while (length > 0) {
        *p = *(s1 + (pos -1));
        p++;
        s1++;
        length--;
    }

    /* zero-terminate our copy */
    *p='\0';

    return r;
}


/*----------------------------------------------------------
*  Function:    concat()
*
*  Description: This function concatenates 2 strings.
*
*  On Entry:    s1 = The first string
*               s2 = The second string to be appended
*
*  Returns:     (char *) pointer to the joined string
*
*---------------------------------------------------------*/
char *concat( char *s1, char *s2 )
{
    int len = strlen(s1) + strlen(s2) + 1;
    char *p = (char *) malloc( len );
    char *r = p;

    /* Copy s1 into p */
    while ( *s1 != '\0') {
        *p = *s1;
        p++;
        s1++;
    };

    /* Then copy s2 into p */
    while ( *s2 != '\0') {
        *p = *s2;
        p++;
        s2++;
    };

    /* zero-terminate our copy */
    *p = '\0';

    return r;
}


/*----------------------------------------------------------
*  Function:    char *ltrim( char *s )
*
*  Description: This removes leading blanks, tabs and
*               line feed in a string.
*
*  On Entry:    Address of the string to trim
*
*  Returns:     ltrimmed string
*---------------------------------------------------------*/
char *ltrim( char *s )
{
    if (!s) return NULL;    /* handle NULL string */
    if (!*s) return s;      /* return empty string */

    while(isspace(*s)) s++;

    return s;
}


/*----------------------------------------------------------
*  Function:    char *rtrim( char *s )
*
*  Description: This removes trailing blanks, tabs and
*               line feed in a string.
*
*  On Entry:    Address of the string to trim
*
*  Returns:     rtrimmed string
*---------------------------------------------------------*/
char *rtrim( char *s )
{
	char *p;

    if (!s) return NULL;        /* handle NULL string */
    if (!*s) return s;          /* return empty string */

    p = s + strlen(s);    /* point to the end of s */

    while(isspace(*--p));       /* continue to point back while it's whitespace */

    /* zero-terminate the string. Take note that this also affects s */
    *(p+1) = '\0';

    return s;
}


/*----------------------------------------------------------
*  Function:    char *trim( char *s )
*
*  Description: This removes leading and trailing blanks,
*               tabs and line feed in a string.
*
*  On Entry:    Address of the string to trim
*
*  Returns:     address of trimmed string
*---------------------------------------------------------*/
char *trim( char *s )
{
	if (!s) return NULL;        /* handle NULL string */
    if (!*s) return s;          /* return empty string */

    return rtrim(ltrim(s));
}


/*----------------------------------------------------------
*  Function:    ucase( char *s )
*
*  Description: Converts the given string to uppercase
*
*  On Entry:    Address of the string to convert
*
*  Returns:     (char *) pointer to uppercase version
*---------------------------------------------------------*/
char *ucase( char *s )
{
    char *p = (char *) malloc( strlen(s) + 1);
    char *r = p; /* Save the original position */

	while(*s != '\0') {
		*p = toupper(*s);
        p++;
        s++;
	}

    /* zero-terminate our copy */
    *p='\0';

    return r;
}


/*----------------------------------------------------------
*  Function:    lcase( char *s )
*
*  Description: Converts the given string to lowercase
*
*  On Entry:    Address of the string to convert
*
*  Returns:     (char *) pointer to lowercase version
*---------------------------------------------------------*/
char *lcase( char *s )
{
    char *p = (char *) malloc( strlen(s) + 1);
    char *r = p; /* Save the original position */

	while(*s != '\0') {
		*p = tolower(*s);
        p++;
        s++;
	}

    /* zero-terminate our copy */
    *p='\0';

    return r;
}


/*----------------------------------------------------------
*  Function:    pcase( char *s )
*
*  Description: Converts the given string to Propercase
*
*  On Entry:    Address of the string to convert
*
*  Returns:     (char *) pointer to Propercase version
*---------------------------------------------------------*/
char *pcase( char *s )
{
	char *p, *r;
	int i = 0;

	s = trim( s );
    p = (char *) malloc( strlen(s) + 1);
    r = p; /* Save the original position */

	while(*s != '\0') {
		if (i == 0)
			*p = toupper(*s);
		else
			*p = tolower(*s);
        p++;
        s++;
		i++;
	}

    /* zero-terminate our copy */
    *p='\0';

    return r;
}


/*----------------------------------------------------------
*  Function:    isthere( char *s, char *r )
*
*  Description: Search s if there is any occurrence of
*               characters listed in r
*
*  On Entry:    s = address of the string to be searched
*               r = address of the string to search
*
*  Returns:     (bool) true = found, false = not found
*---------------------------------------------------------*/
bool isthere ( char *s, char *r )
{
	while(*r != '\0') {
		char c = *r;
		if ( strchr(s, c) == NULL ) return false;
		r++;
	}
    return true;
}


/*----------------------------------------------------------
*  Function:    intToString( int n )
*
*  Description: Converts integer to string
*
*  On Entry:    n = integer to convert
*
*  Returns:     (char *) string equivalent
*---------------------------------------------------------*/
char *intToString( int n )
{
    char ls[20];
    sprintf(ls, "%d", n);
    return trim(ls);
}


/*----------------------------------------------------------
*  Function:    strcut( char *s, int max )
*
*  Description: Truncates s (from left) to max number of characters
*               given by max. If max is longer, no truncation
*               occurs.
*
*  On Entry:    s = Address of the string to truncate
*               imax = number of maximum charaters
*
*  Returns:     (char *) pointer to truncated version
*---------------------------------------------------------*/
char *left( char *s, int max )
{
	char *p, *r;

	int len = strlen(s);
    p = (char *) malloc( len + 1);
    r = p; /* Save the original position */

	if (max >= len)
	{
		/* No need to null terminate, already is */
		strcpy(r, s);
	}
	else
	{
		strncpy(r, s, len);
		p += max;

		/* zero-terminate our copy */
		*p='\0';
	}

    return r;
}


/*---------------------------------------------------------------
*  Function:		char *wpl( char *s, int n )
*
*  Description:		Words-Per-Line. This function splits a string
*                   n words per line.
*
*  Parameter(s):    s = pointer to the string
*                   n = number of words per line
*
*  Returns:         (char *) words-per-line delimeted version
*                   NULL if *s points to empty string
*---------------------------------------------------------------*/
char *wpl( char *s, int n )
{
	register int i, j, c, len;
	char *p;
	char *buffer;
	
	len = strlen( s );

	if ( !s || len == 0)
	{
		/* Message is empty */
		return NULL;
	}

	p = malloc( MAX_CHARS_PER_LINE * sizeof(char) );
	buffer = malloc( (len + n + 1) * sizeof(char) );

	/* Initialize variable(s) */
	j=0;
	c=0;

	/* Loop through the entire string */
    for ( i=0; s[i] != '\0'; i++ )
    {
		p[j] = s[i];        
		if (s[i] == ' ') {
            c++;
			if ( c == n )
			{
				p[j] = '\0';
				sprintf(p, "%s\n", p);
				buffer = concat( buffer, p );
				c = 0;
				j = 0;
			}
			else j++;
		}
		else j++;
    }
	p[j]='\0';
	sprintf(p, "%s", p);
	buffer = concat( buffer, p );

	free(p);
	return buffer;
}


/*---------------------------------------------------------------
*  Function:        int nfgets (char* *ln, FILE *f )
*
*  Description:     Replacement to fgets
*
*  Parameter(s):    ln = pointer to a string pointer
*                   f = soure (could be a real file, stdin, stdout)
*
*  Returns:         int (0=ERROR, >0 = OK)
*---------------------------------------------------------------*/
int nfgets(char* *ln, FILE *f)
{
    enum {NOMEM = 0, OK};

    int     cursize, ch, i;
    char   *buffer, *temp;

    *ln = NULL; /* default */
    if (NULL == (buffer = malloc(INITSIZE))) return NOMEM;
    cursize = INITSIZE;

    i = 0;
    while ((EOF != (ch = getc(f))) && ('\n' != ch)) {
        if (i >= (cursize - 1)) { /* extend buffer */
            cursize += DELTASIZE;
            if (NULL == (temp = realloc(buffer, (size_t)cursize))) {
                /* ran out of memory, return partial line */
                buffer[i] = '\0';
                *ln = buffer;
                return NOMEM;
            }
            buffer = temp;
        }
        buffer[i++] = ch;
    }
    if ((EOF == ch) && (0 == i)) {
        free(buffer);
        return EOF;
    }

    buffer[i] = '\0';
    if (NULL == (temp = realloc(buffer, (size_t)i + 1))) {
            *ln = buffer;  /* without reducing it */
    }
    else *ln = temp;
    return OK;
}


/*---------------------------------------------------------------
*  Function:        void nswap(void *a, void *b, size_t size)
*
*  Description:     Counts the elements in a given array
*
*  Parameter(s):    void *arr = address of array
*                   size_t data_size = data type size
*
*  Returns:         int = count of array, if error occurs -1
*
*  Usage:           nswap(&a, &b, sizeof(char *))
*                   nswap(&a, &b, sizeof(int))
*
*---------------------------------------------------------------*/
void nswap(void *a, void *b, size_t size)
{
    void *t = malloc(size);

    memcpy(t, a, size);
    memcpy(a, b, size);
    memcpy(b, t, size);

    free(t);
}
