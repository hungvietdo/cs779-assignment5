
#include "functions.h"

main(argc, argv )
int argc;
char *argv[];

{
    int	sd;
    struct	sockaddr_in server;
    struct  hostent *hp, *gethostbyname();

    if (argc !=3) {
        printf("Usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }


    sd = socket (AF_INET,SOCK_STREAM,IPPROTO_SCTP);

    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    bcopy ( hp->h_addr, &(server.sin_addr.s_addr), hp->h_length);
    server.sin_port = htons(atoi(argv[2]));
    connect(sd, (SA *) &server, sizeof(server));
    for (;;) {
        send(sd, "S HI", 5, 0 );
        printf("sent S HI\n");
        sleep(2);
    }
}
