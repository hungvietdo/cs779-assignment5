# environment variables setup
CC		=	gcc

PROGS =   Qclient

CFLAGS          = -w -pthread

all: $(PROGS)

	
Qclient:   
	$(CC) $(CFLAGS) -o Qclient Qclient.c functions.c

clean:
	rm -f Qclient