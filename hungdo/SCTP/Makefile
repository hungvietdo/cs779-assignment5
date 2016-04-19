PROGS = Qserver Qclient Sserver Sclient GCServer
	 
LIBS    = -w -lnsl  -lsctp -pthread

all:    ${PROGS}
	@chmod 755 *

GCServer: GCServer.c
	gcc  -o GCServer GCServer.c $(LIBS) functions.c

Sserver: Sserver.c
	gcc  -o Sserver Sserver.c $(LIBS)
Sclient: Sclient.c
	gcc  -o Sclient Sclient.c $(LIBS)

Qserver: Qserver.c
	gcc  -o Qserver Qserver.c $(LIBS)
Qclient: Qclient.c
	gcc  -o Qclient Qclient.c $(LIBS)


clean:
	rm -f $(PROGS) *.o core

