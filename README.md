# [cip](https://github.com/nutsbox/cip/)
<pre>
cip v0.53 - tool for encryption, decryption, and hashing of given string or file
by Nestor A. Jaba-an <a href="mailto:nestor@nutsbox.ph">nestor@nutsbox.ph</a>, 2019. Public Domain.

<b>NOTE:</b> I consider cip itself as public domain (PD), but I used some libraries that
are not public domain and due credit and license applies to these libraries.

<b>CREDITS:</b>
  - Tom St Denis, LibTomCrypt v1.18.2 (PD), <a href="https://github.com/libtom/libtomcrypt">https://github.com/libtom/libtomcrypt</a>
  - Tom St Denis, LibTomMath v1.2.0 (PD), <a href="https://github.com/libtom/libtommath">https://github.com/libtom/libtommath</a>
  - Daniel Richards, hashsum.c & ltcrypt.c (PD), <a href="mailto:kyhwana@world-net.co.nz">kyhwana@world-net.co.nz</a>
  - Majek, <a href="https://github.com/majek/csiphash/blob/master/csiphash.c">https://github.com/majek/csiphash/blob/master/csiphash.c</a> (MIT Lic.)
  - Aumasson, J. P., & Bernstein, D. J., SipHash (PD)
  - Rxi, <a href="https://github.com/rxi/uuid4">https://github.com/rxi/uuid4</a> (MIT Lic.)
  - John Walker, base64-1.5 (PD), <a href="http://www.fourmilab.ch/webtools/base64/index.html">http://www.fourmilab.ch/webtools/base64/index.html</a>

<b>USAGE:</b>
usage:	cip [OPTIONS [SUB-PARAM]...]
encrypts, decrypts and hashes key for a given string or file

-s    &lt;string&gt;		the string to encrypt, decrypt or to hash
-f    &lt;filename&gt;	the file to encrypt, decrypt or to hash

-t*   [algorithm]	where '*' should be replaced by 's' or 'f'
			performs hash on given string (-ts) or filename (-tf)
			supported algorithms: md2 md4 md5 sha1 sha224 sha256
			sha384 sha512 sha512-224 sha512-256 sha3-224
			sha3-256 (default) sha3-384 sha3-512 blake2b-160
			blake2b-256 blake2b-384 blake2b-512 whirlpool tiger
			blake2s-128 blake2s-160 blake2s-224 blake2s-256 rmd128
			rmd160 rmd256 rmd320 chc_hash siphash crc32 adler32
			shake3-128 shake3-256
      
-len  &lt;length&gt;		custom hash length (in bytes) for shake3-128/256.
			defaults: shake3-128 (16 bytes), shake3-256 (32 bytes)

-k    &lt;key&gt;		key as required by the following:
			- siphash (i.e., SipHash-2-4): 16 characters
			- ciphers (see below). if not given, user will be prompted
			to provide a key--no specific number of characters
			required for cipher (think of this like a password)

-b*   [outfile]		where '*' should be replaced by 'e' to encode
			or 'd' to decode in base64 the given string through '-s'
			or file through '-f' options. if outfile is not specified,
			result will be dump to the screen (i.e., stdout)

-u[*]			where '*' may be replaced by 's' or 'f'
			generates uuid4 randomly (-u) or from given string (-us)
			or filename (-uf)

-e*  [outfile]		where '*' should be replaced by 's' or 'f'
-d*  [outfile]		encrypts or decrypts a string (-es|-ds)
			given through '-s' or file (-ef|-df) given through '-f'
			and outputs to screen or to [outfile]. if '-c' is not
			specified, aes is the cipher used by default.

-c   &lt;cipher&gt;		cipher to use for encryption/decryption.
			ciphers: aes (default) blowfish xtea rc2 rc5 rc6 twofish
			safer+ safer-k64 safer-sk64 safer-k128 safer-sk128
			des 3des cast5 noekeon skipjack anubis khazad
			seed kasumi multi2 camellia

-a   &lt;algorithm&gt;	set hashing algorithm to be used for.
			encryption/decryption (see above -t* option for list)

-v			version info
-h			help (this page)
</pre>
## Compilation
cip was developed in C, can be compiled with gcc or clang (statically, for easy distribution of binary) and tested to run in windows, linux (ubuntu 18.0, centos 7.x, termux in adroid), FreeBSD-11 and macOS Yosemite / Mojave / Catalina.
<p><b>NOTE:</b><br>
First, you need to download or clone libtommath and libtomcrypt libraries as given by the links above (see CREDITS) and save them into lib folder to correspond with the instructions below; otherwise you need to adjust. Anyway, the point is that these libraries must be prepared first before compiling cip.
</p>
<ol>
<li>Compile libtommath first as this is required by libtomcrypt.
<pre>
cd lib/libtommath-1.2.0/
make -f &lt;os_specific_makefile&gt;
sudo make -f &lt;os_specific_makefile&gt; install clean
</pre>
</li>
<li>Compile libtomcrypt library
<pre>
cd lib/libtomcrypt-1.18.2
make -f &lt;os_specific_makefile&gt;
sudo make -f &lt;os_specific_makefile&gt; install
### For macOS, we copy the generated libtomcrypt.a into lib folder for ease of static compilation
cp libtomcrypt.a ../.
make clean
</pre>
</li>
<li>If you want to compile against shared or dynamic library, you need to modify the corresponding makefile to remove static-compilation-related options.
</li>
<li>Compile cip. Use corresponding Makefile. The accompanying makefiles are: makefile (windows), makefile.nix (linux/unix), and makefile.osx (macOS)
<pre>
   Windows: mingw32-make
Linux/Unix: make -f makefile.nix
     macOS: make -f makefile.osx
   FreeBSD: gmake -f makefile.nix ### because the default make utility in freeBSD has 
                                  ### trouble interpreting our makefile
                                  ### hence we use gmake instead.
</pre>
</li>
</ol>

## Installation
If you opted to use the binary version, simply download the corresponding binary file from the bin directory into your local directory.
