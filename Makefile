CC	 	= gcc
LD	 	= gcc
CFLAGS	 	= -Wall -g

LDFLAGS	 	= 
DEFS 	 	=

all:	server client_num

server: server.c
	$(CC) $(DEFS) $(CFLAGS) $(LIB) -o server server.c

client_num: client_num.c
	$(CC) $(DEFS) $(CFLAGS) $(LIB) -o client_num client_num.c

clean:
	rm -f *.o
	rm -f *~
	rm -f core.*
	rm -f server
	rm -f client_num
