#include "functions.h"

main(argc, argv )
int argc;
char *argv[];
{
    int	sd;
    struct	sockaddr_in server;
    struct  hostent *hp, *gethostbyname();
    int i;

    if (argc !=3) {
        printf("Usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }


    sd = socket (AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    bcopy ( hp->h_addr, &(server.sin_addr.s_addr), hp->h_length);
    server.sin_port = htons(atoi(argv[2]));
        if ( connect(sd, (SA *) &server, sizeof(server)) < 0 ) {
            printf("connecting to server  failed\n");
        }


    for (;;) {
        sendto(sd, "Q HI",5, 0, (SA *) &server, sizeof(server));
        printf("sendto Q HI\n");
        sleep(2);
    }
}
