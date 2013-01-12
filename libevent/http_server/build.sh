CFLAGS="-Wall -Werror"
CPPFLAGS="-I/opt/libevent-2.0.20-stable/include"
LDFLAGS="-L/opt/libevent-2.0.20-stable/lib"
LIBS="-levent"
gcc -g -o http_server http_server.c $CFLAGS $CPPFLAGS $LDFLAGS $LIBS
