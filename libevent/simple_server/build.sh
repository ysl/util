CFLAGS="-Wall -Werror"
CPPFLAGS="-I/opt/libevent-2.0.20-stable/include"
LDFLAGS="-L/opt/libevent-2.0.20-stable/lib"
LIBS="-levent"
gcc -o simple_server simple_server.c $CFLAGS $CPPFLAGS $LDFLAGS $LIBS
