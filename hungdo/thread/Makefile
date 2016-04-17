# environment variables setup
CC		=	gcc

PROGS =   TUMServer MClient TClient UClient

CFLAGS          = -w -pthread

all: $(PROGS)

	
TUMServer:   
	$(CC) $(CFLAGS) -o TUMServer TUMServer.c functions.c

MClient:
	$(CC) $(CFLAGS) -o MClient MClient.c functions.c

TClient:
	$(CC) $(CFLAGS) -o TClient TClient.c functions.c
	
UClient:
	$(CC) $(CFLAGS) -o UClient UClient.c functions.c
	

clean:
	rm -f TUMServer UClient TClient MClient

