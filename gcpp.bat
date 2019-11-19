@echo off
echo.

IF EXIST %1.cpp GOTO COMPILE
echo Error: file %1.cpp not found
echo.
echo USAGE:  gcpp <progname> [release]
echo          release - (lowercase) means without debugging information
echo                    if not specified, debug version is assumed.
GOTO END

:COMPILE
IF "%2" == "release" GOTO RELEASE
g++ -Wl,--enable-auto-import %1.cpp -o"%1.exe" -Llib -lchilkat-9.5.0 -L/c/MinGW/lib -lcrypt32 -lws2_32 -ldnsapi
GOTO END

:RELEASE
g++ -g0 -O3 -s -Wl,--enable-auto-import %1.cpp -o"%1.exe" -Llib -lchilkat-9.5.0 -L/c/MinGW/lib -lcrypt32 -lws2_32 -ldnsapi

:END
