#ifndef B64_H_INCLUDED
#define B64_H_INCLUDED


void encode(FILE *fi, FILE *fo);
void decode(FILE *fi, FILE *fo);

char *s_encode(char *in);
char *s_decode(char *in);

void  f_encode(char *infile, char *outfile);
void  f_decode(char *infile, char *outfile);

void sf_encode(char *in, char *outfile);
void sf_decode(char *in, char *outfile);

#endif // B64_H_INCLUDED
