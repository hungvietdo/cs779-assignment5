CC              =       gcc 
RM              = rm -rf 

# libraries and includes
CFLAGS          = $(NORMCFLAGS)
LIBS       = -lnsl  -lpthread

PROGS = QClient SClient TClient UClient MClient GCServer 

all: $(PROGS)

QClient:  QClient.o
	$(CC) $(CFLAGS) -o QClient QClient.c $(LIBS)
SClient:  SClient.o
	$(CC) $(CFLAGS) -o SClient SClient.c $(LIBS)
TClient:  TClient.o
	$(CC) $(CFLAGS) -o TClient TClient.c $(LIBS)
UClient:  UClient.o
	$(CC) $(CFLAGS) -o UClient UClient.c $(LIBS)
MClient:  MClient.o
	$(CC) $(CFLAGS) -o MClient MClient.c $(LIBS)
GCServer:  GCServer.o
	$(CC) $(CFLAGS) -o GCServer GCServer.c $(LIBS)
clean:;
	rm -f $(PROGS) core *.o
