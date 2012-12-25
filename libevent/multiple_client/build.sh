CFLAGS="-Wall -Werror"
CPPFLAGS="-I/opt/libevent-2.0.20-stable/include"
LDFLAGS="-L/opt/libevent-2.0.20-stable/lib"
LIBS="-levent"
gcc -g -o multiple_client multiple_client.c $CFLAGS $CPPFLAGS $LDFLAGS $LIBS
