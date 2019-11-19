#include "include/cipher.h"


char * krypt (bool is_file, bool is_encrypt, char *in, char *out, char *cipher, char *inkey, char *algo)
{
	unsigned char key[MAXBLOCKSIZE], IV[MAXBLOCKSIZE];
	unsigned char inbuf[512], plaintext[512], ciphertext[512];
	FILE *fi = NULL, *fo = NULL;
	char *rtns = NULL;
	int cipher_idx, key_size, err;
	unsigned long ivsize, outlen, x, y;
	bool f2f = false;
	
	prng_state prng;
	symmetric_CTR ctr;

/*
	printf("   is_file: %d\n", is_file);
	printf("is_encrypt: %d\n", is_encrypt);
	printf("        in: %s\n", in);
	printf("       out: %s\n", out);
	printf("    cipher: %s\n", cipher);
	printf("     inkey: %s\n", inkey);
	printf("      algo: %s\n", algo);
*/

	/* register ciphers, hashes, and psuedo random generators */
	register_all_ciphers();
	register_all_prngs();


	/* if is_file, 'in' is interpreted as filename */
	if (is_file) fi = fopen(in, "rb");
	else fi = tmpfile();
	if (fi == NULL) die(133, in);

	/* put 'in' to fi */
	if (!is_file) {
		for (int i=0; i < strlen(in); i++) fputc(in[i], fi);
		rewind(fi);
	}

	/* determine if out is a file or to be returned */
	if (out) {
		fo = fopen(out, "wb");
		if (is_file) f2f = true;
	}
	else fo = stdout;
	if (fo == NULL) die(133, in);

	/* get cipher index */
	cipher_idx = find_cipher(cipher);
	if (cipher_idx == -1) die(134, cipher);

	/* get ivsize--should be equal to the block_length of cipher */
	ivsize = cipher_descriptor[cipher_idx].block_length;

	/* hash the key into key buffer */
	char *h = (char *) gethash(algo, inkey, &outlen);
	outlen = ctob(h, key);
	/* printf("     hash1: %s\n", h); */

	if (outlen < cipher_descriptor[cipher_idx].min_key_length) outlen = ctobx(h, key);
	/* printf("     hash1: %s\n", h); */

	/* determine key_size                               */
	/* if selected cipher is multi2, set key size to 40 */
	if (strcmp(cipher, "multi2") == 0) {
		if (outlen < MULTI2_KEYSIZE) die(136, algo);
		key_size = MULTI2_KEYSIZE;
	}
	else key_size = outlen;

	/* ensure key_size is acceptable to selected cipher */
	if (cipher_descriptor[cipher_idx].keysize(&key_size) != CRYPT_OK) die(135, NULL);

	/* encrypt */
	if (is_encrypt) {

		/* Setup fortuna for random bytes for IV */
		if ((err = rng_make_prng(128, find_prng("fortuna"), &prng, NULL)) != CRYPT_OK) {
           die(137, (char *) error_to_string(err));
        }

        x = fortuna_read(IV,ivsize,&prng); /* get random number */
        if (x != ivsize) die(138, NULL);

        /* first data to write is the IV */
        if (fwrite(IV, 1, ivsize, fo) != ivsize) die(139, NULL);

        if ((err = ctr_start(cipher_idx, IV, key, key_size, 0, CTR_COUNTER_LITTLE_ENDIAN,&ctr)) != CRYPT_OK) {
            die(140, (char *) error_to_string(err));
        }

        do {
        	y = fread(inbuf, 1, sizeof(inbuf), fi);

        	if ((err = ctr_encrypt(inbuf, ciphertext, y, &ctr)) != CRYPT_OK) {
               die(141, (char *) error_to_string(err));
            }

            if (fwrite(ciphertext, 1, y, fo) != y) die(142, out);

        } while (y == sizeof(inbuf));
        
        if (f2f) rtns = concat(concat(in, " encrypted -> "), out);

	} else { /* decrypt */
		
		if (fread(IV, 1, ivsize, fi) != ivsize) die(141, in);  /* need to read in IV */

		if ((err = ctr_start(cipher_idx, IV, key, key_size, 0, CTR_COUNTER_LITTLE_ENDIAN, &ctr)) != CRYPT_OK) {
			die(140, (char *) error_to_string(err));
		}

		do {
			y = fread(inbuf, 1, sizeof(inbuf), fi);

			if ((err = ctr_decrypt(inbuf, plaintext, y, &ctr)) != CRYPT_OK) {
				die(144, (char *) error_to_string(err));
         	}

         	if (fwrite(plaintext, 1, y, fo) != y) die(142, out);

		} while (y == sizeof(inbuf));
		
		if (f2f) rtns = concat(concat(in, " decrypted -> "), out);
	}

	fclose(fi);
	fclose(fo);

	return rtns;
}


#if defined (_WIN32) || defined(_WIN64)
char * readkey(char * prompt)
{
	int i=0;
	char *key = (char *) calloc(100, sizeof(char));
	
	if (!key) return NULL;
	
	fprintf(stdout, "%s: ", prompt);
	do {
		key[i]=getch();
		if(key[i] == '\b' && i > 0) {
			fprintf(stdout, "\b \b");
			i--;
			continue;
		} else if(key[i] != '\r' && key[i] != '\b') {
			i++;
			fprintf(stdout, "*");
		}
	} while(key[i] != '\r');

	key[i] = '\0';

	return key;
}
#endif


char * getkey()
{
	char * key;
	char * tk;

	#if defined (_WIN32) || defined(_WIN64)
		key = readkey("  enter key");
		fprintf(stdout, "\n");
		tk =  readkey("confirm key");
		fprintf(stdout, "\n");
	#else
		key = getpass("  enter key: ");
		 tk = getpass("confirm key: ");

		 /* copy the string because getpass releases it afterwards */
		 key = concat("", key);
	#endif

	if (strcmp(key, tk)!=0) die(130, NULL);

	return key;
}


unsigned char * gethash(char *algo, char *in, unsigned long *outlen)
{
	unsigned char *out=NULL;

	/* is this a SHAKE algorithm? */
	if (strcmp(left(algo,6), "shake3") == 0) {
		if (strcmp(algo, "shake3-128") == 0) return (out = getshake(false, 128, in, 16));
		if (strcmp(algo, "shake3-256") == 0) return (out = getshake(false, 256, in, 32));
	} 

	if (strcmp(algo, "crc32") == 0) return (out = (unsigned char *) crc32(false, in));
	if (strcmp(algo, "adler32") == 0) return (out = (unsigned char *) adler32(false, in));

	/* Perform the non-SHAKE hash and exit */
	char * key = NULL;
	if (strcmp(algo, "siphash") == 0) {
		/* first, generate 16-bytes key as required by siphash */
		key = (char *) getshake(false, 128, in, 8);
	}
	
	out = (unsigned char *) hash(in, algo, false, key);
	*outlen = strlen((char *) out);

	return out;
}



int ctob(char* chars, unsigned char * bytes)
{
	int j=0;
	for( int i = 0; i < strlen(chars); i+=2 ) {
        unsigned char c = ctod(chars[i]);
        c = c << 4;
        c += ctod(chars[i+1]);
        bytes[j++] = c;
    }
    bytes[j] = '\0';

    return j;
}


int ctobx(char * chars, unsigned char * bytes)
{
	int i=0;
    for(i = 0; i < strlen(chars); i++)
        bytes[i] = (unsigned char) chars[i];
    bytes[i] = '\0';
    return i;
}


/* Youness Nachid-Idrissi, https://stackoverflow.com/questions/31079978/how-to-convert-hexadecimal-to-decimal */
int ctod(char c)
{
	if(c>='0' && c<='9') return c-'0';
	if(c>='a' && c<='f') return c-'a'+10;
	if(c>='A' && c<='F') return c-'A'+10;
	return 0;
}


// Return a name of platform, if determined, otherwise - an empty string
char *get_platform_name() {
	return (PLATFORM_NAME == NULL) ? "" : PLATFORM_NAME;
}
