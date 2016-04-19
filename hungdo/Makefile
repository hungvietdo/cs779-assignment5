PROGS = GCServer Qclient
	 
LIBS    = -w -lnsl  -lsctp -pthread

all:    ${PROGS}
	@chmod 755 *

GCServer: GCServer.c
	gcc  -o GCServer GCServer.c $(LIBS) functions.c
Qclient: Qclient.c
	gcc  -o Qclient Qclient.c $(LIBS)


clean:
	rm -f $(PROGS) *.o core

