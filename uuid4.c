/**
 * Copyright (c) 2018 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(_WIN32)
#include <windows.h>
#include <wincrypt.h>
#endif

#include "include/uuid4.h"
#include "include/hash.h"


static uint64_t seed[2];


static uint64_t xorshift128plus(uint64_t *s) {
  /* http://xorshift.di.unimi.it/xorshift128plus.c */
  uint64_t s1 = s[0];
  const uint64_t s0 = s[1];
  s[0] = s0;
  s1 ^= s1 << 23;
  s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
  return s[1] + s0;
}


int uuid4_init(void) {
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
  int res;
  FILE *fp = fopen("/dev/urandom", "rb");
  if (!fp) {
    return UUID4_EFAILURE;
  }
  res = fread(seed, 1, sizeof(seed), fp);
  fclose(fp);
  if ( res != sizeof(seed) ) {
    return UUID4_EFAILURE;
  }

#elif defined(_WIN32)
  int res;
  HCRYPTPROV hCryptProv;
  res = CryptAcquireContext(
    &hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
  if (!res) {
    return UUID4_EFAILURE;
  }
  res = CryptGenRandom(hCryptProv, (DWORD) sizeof(seed), (PBYTE) seed);
  CryptReleaseContext(hCryptProv, 0);
  if (!res) {
    return UUID4_EFAILURE;
  }

#else
  #error "unsupported platform"
#endif
  return UUID4_ESUCCESS;
}


void uuid4_generate(char *dst) {
  static const char *template = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
  static const char *chars = "0123456789abcdef";
  union { unsigned char b[16]; uint64_t word[2]; } s;
  const char *p;
  int i, n;
  /* get random */
  s.word[0] = xorshift128plus(seed);
  s.word[1] = xorshift128plus(seed);

  /* build string */
  p = template;
  i = 0;
  while (*p) {
    n = s.b[i >> 1];
    n = (i & 1) ? (n >> 4) : (n & 0xf);
    switch (*p) {
      case 'x'  : *dst = chars[n];              i++;  break;
      case 'y'  : *dst = chars[(n & 0x3) + 8];  i++;  break;
      default   : *dst = *p;
    }
    dst++, p++;
  }
  *dst = '\0';
}


/*----------------------------------------------------------
*  Function:    uuid4_gets()
*
*  Description: Generates uuid4 from input file or string
*
*  On Entry:    is_file = true if file, otherwise false
*               in = if file, contains file path and name
*                    otherwise it contains the string to
*                    generate uuid4 from.
*
*  Uses:        to_uuid4()
*
*  Returns:     (char *) the corresponding uuid4
*
*  Author:      Nestor A. Jaba-an, 24-Oct-2019.
*               <nestor@nutsbox.ph>
*
*---------------------------------------------------------*/
char * uuid4_gets(bool is_file, char *in) 
{
  
  unsigned char * out = NULL;
  char * uuid = (char *) malloc(UUID4_LEN * sizeof(char));

  if (is_file) {  /* in contains the file path and name */
    out = getshake(true, 256, in, UUID4_LEN);
  }
  /* in contains the string to generate uuid4 from */
  else out = getshake(false, 256, in, UUID4_LEN);

  to_uuid4(out, uuid);

  return uuid;
}


/*----------------------------------------------------------
*  Function:    to_uuid4()
*
*  Description: The function that actually generates uuid4
*               from the given input string.
*
*  On Entry:    in = contains the string to generate uuid4 from.
*
*  Author:      Nestor A. Jaba-an, 24-Oct-2019.
*               <nestor@nutsbox.ph>
*
*---------------------------------------------------------*/
void to_uuid4(unsigned char * in, char *out)
{
  static const char *template = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
  static const char *chars = "0123456789abcdef";

  union { unsigned char b[16]; uint64_t word[2]; } s;
  const char *p;

  unsigned char key[16];
  hash_state md;

  /* initialize md5 */
  md5_init(&md);
  
  /* generate a 16-byte md5 hash key from input */
  md5_process(&md, in, strlen((char *)in));
  md5_done(&md, key);

  /* Initialize seed based by siphashing */
  seed[0] = siphash24(in, strlen((char *)in), (const char*) key);  /* Generate uint64_t by siphashing the input string */
  seed[1] = siphash24(key, sizeof(key), (const char*)key);        /* Generate uint64_t by siphashing the key itself */

  s.word[0] = xorshift128plus(seed);
  s.word[1] = xorshift128plus(seed);

  p = template;
  int i=0, n;

  while (*p) {
    n = s.b[i >> 1];
    n = (i & 1) ? (n >> 4) : (n & 0xf);
    switch (*p) {
      case 'x'  : *out = chars[n];              i++;  break;
      case 'y'  : *out = chars[(n & 0x3) + 8];  i++;  break;
      default   : *out = *p;
    }
    out++, p++;
  }
  *out = '\0';
}
