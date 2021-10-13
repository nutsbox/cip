## Welcome to cip

You can use cip as an alternative tool for encryption, decryption, B64 encoding/decoding, and hashing of given string or file.

### Features
* [String or file hashing](#string-or-file-hashing)
* String or file encryption / decryption
* String or file B64 encoding / decoding
* UUID generation
* UUID generation based on given string or file (maybe considered as UUID hash)
* Cross-platform: cip has been tested to run on Windows (XP, 7, 10), Linux (Ubuntu 18.0, Centos 7.x and above, Termux in Android), FreeBSD-11 and Mac OSX (Yosemite, Mojave, Catalina). You may need to recompile if you want to port it to other operating system.

### License
I consider cip, itself, as Public Domain (PD). But I used some libraries that are not public domain and due credit and license applies to these libraries.

### Credits
- Tom St Denis, LibTomCrypt v1.18.2 (PD), [https://github.com/libtom/libtomcrypt](https://github.com/libtom/libtomcrypt "LibTomCrypt")
- Tom St Denis, LibTomMath v1.2.0 (PD), [https://github.com/libtom/libtommath](https://github.com/libtom/libtommath "LibTomMath")
- Daniel Richards, hashsum.c & ltcrypt.c (PD), [kyhwana@world-net.co.nz](kyhwana@world-net.co.nz "hashsum and ltcrypt")
- Majek, csiphash.c (MIT Lic.), [https://github.com/majek/csiphash/blob/master/csiphash.c](https://github.com/majek/csiphash/blob/master/csiphash.c "csiphash.c")
- [Aumasson, J. P.](https://www.aumasson.jp/ "JP Aumasson") and [Bernstein, D. J.](https://cr.yp.to/djb.html, "DJ Bernstein"), SipHash Algorithm (PD)
- Rxi, uuid4 (MIT Lic.), [https://github.com/rxi/uuid4](https://github.com/rxi/uuid4 "uuid4")
- John Walker, base64-1.5 (PD), [http://www.fourmilab.ch/webtools/base64/index.html](http://www.fourmilab.ch/webtools/base64/index.html "base64-1.5")

### Usage
`cip` commands are consistent across different supported OS.
#### Default
Simply typing `cip` at the command line prompt would give you the following:
```markdown
option(s) required.
type "cip -h" for help
```
#### Help
`cip -h`
```markdown
usage: cip [OPTIONS [SUB-PARAM]...]
encrypts, decrypts and hashes key for a given string or file

-s   <string>          the string to encrypt, decrypt or to hash
-f   <filename>        the file to encrypt, decrypt or to hash


 -t*  [algorithm]       where '*' should be replaced by 's' or 'f'
                        performs hash on given string (-ts) or filename (-tf)
                        supported algorithms: md2 md4 md5 sha1 sha224 sha256
                        sha384 sha512 sha512-224 sha512-256 sha3-224
                        sha3-256 (default) sha3-384 sha3-512 blake2b-160
                        blake2b-256 blake2b-384 blake2b-512 whirlpool tiger
                        blake2s-128 blake2s-160 blake2s-224 blake2s-256 rmd128
                        rmd160 rmd256 rmd320 chc_hash siphash crc32 adler32
                        shake3-128 shake3-256

 -len <length>          custom hash length (in bytes) for shake3-128/256.
                        defaults: shake3-128 (16 bytes), shake3-256 (32 bytes)

 -k   <key>             key as required by the following:
                        - siphash (i.e., SipHash-2-4): 16 characters
                        - ciphers (see below). if not given, user will be prompted
                        to provide a key--no specific number of characters
                        required for cipher (think of this like a password)

 -b*  [outfile]         where '*' should be replaced by 'e' to encode
                        or 'd' to decode in base64 the given string through '-s'
                        or file through '-f' options. if outfile is not specified,
                        result will be dump to the screen (i.e., stdout)

 -u[*]                  where '*' may be replaced by 's' or 'f'
                        generates uuid4 randomly (-u) or from given string (-us)
                        or filename (-uf)

 -e*  [outfile]         where '*' should be replaced by 's' or 'f'
 -d*  [outfile]         encrypts or decrypts a string (-es|-ds)
                        given through '-s' or file (-ef|-df) given through '-f'
                        and outputs to screen or to [outfile]. if '-c' is not
                        specified, aes is the cipher used by default.

 -c   <cipher>          cipher to use for encryption/decryption.
                        ciphers: aes (default) blowfish xtea rc2 rc5 rc6 twofish
                        safer+ safer-k64 safer-sk64 safer-k128 safer-sk128
                        des 3des cast5 noekeon skipjack anubis khazad
                        seed kasumi multi2 camellia

 -a   <algorithm>       set hashing algorithm to be used for.
                        encryption/decryption (see above -t* option for list)

 -v                     version info
 -h                     help (this page)
```
#### String or file hashing
`cip` supports the following hashing algorithms: md2, md4, md5, sha1, sha224, sha256, sha384, sha512, sha512-224, sha512-256, sha3-224, **sha3-256** (_default_), sha3-384, sha3-512, blake2b-160, blake2b-256, blake2b-384, blake2b-512, whirlpool, tiger, blake2s-128, blake2s-160, blake2s-224, blake2s-256, rmd128, rmd160, rmd256, rmd320, chc_hash, siphash, crc32, adler32, shake3-128, and shake3-256.
##### Hash a string using sha3-256 (default) Algorithm
```markdown
C:\>cip -s "Hello World!" -ts
d0e47486bbf4c16acac26f8b653592973c1362909f90262877089f9c8a4536af
```
The output of `cip -h` shows that to select an algorithm, you have to use `-t*` followed by the name of the algorithm. If no name is provided, `cip` defaults to **sha3-256**. The `*` is to be replaced by either `s` or `f`. If you are hashing a string, the option becomes `-ts`. To hash a file, the option becomes `tf`. The following examples depict this point:
##### Hash a string using md5 algorith
```markdown
C:\>cip -s "Hello World!" -ts md5
ed076287532e86365e841e92bfc50d8c
```
Not changing  `-t*` to either `-ts` or `-tf` results to an error:
```markdown
C:\>cip -s "Hello World!" -t*
no given operation(s) to perform
type "cip -h" for help

C:\>cip -s "Hello World!" -t* md5
no given operation(s) to perform
type "cip -h" for help

C:\>cip -s "Hello World!"
no given operation(s) to perform
type "cip -h" for help
```

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://docs.github.com/categories/github-pages-basics/) or [contact support](https://support.github.com/contact) and we’ll help you sort it out.
