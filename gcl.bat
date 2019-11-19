@echo off
echo.

IF EXIST %1.c GOTO COMPILE
echo Error: file %1.c not found
echo.
echo "USAGE:  gcl <progname> [release]"
echo          release - (lowercase) means without debugging information
echo                    if not specified, debug version is assumed.
GOTO END

:COMPILE
IF "%2" == "release" GOTO RELEASE
gcc -c %1.c -o"%1.o"
g++ %1.o -o"%1.exe" -Llib -lchilkat-9.5.0 -L/c/MinGW/lib -lcrypt32 -lws2_32 -ldnsapi 
GOTO END

:RELEASE
gcc -g0 -O3 -c %1.c -o"%1.o"
strip --strip-unneeded %1.o
g++ -g0 -O3 -s %1.o -o"%1.exe" -Llib -lchilkat-9.5.0 -L/c/MinGW/lib -lcrypt32 -lws2_32 -ldnsapi

:END
