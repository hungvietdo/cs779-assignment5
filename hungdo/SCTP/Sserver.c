#include "functions.h"
main(int argc, char *argv[])
{
    int sd, psd;
    struct   sockaddr_in name;
    struct   sockaddr_in cliaddr;
    char   buf[1024];
    int    cc;
    int len;

    if (argc !=2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(0);
    }

    sd = socket (AF_INET,SOCK_STREAM,IPPROTO_SCTP);
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    name.sin_port = htons(atoi(argv[1]));

    if (   bind( sd, (SA *) &name, sizeof(name) ) < 0 ) {
        printf("error binding\n");
        exit(-1) ;
    }

    listen(sd,1);
    psd = accept(sd, 0, 0);
    len = sizeof(cliaddr);
    getpeername(psd, (SA *) &cliaddr, &len);
    printf("remote addr: %s,  port %d\n",
           inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)),
           ntohs(cliaddr.sin_port));

    for(;;) {
        cc=read(psd,buf,sizeof(buf)) ;
        if (cc == 0) exit (0);
        buf[cc] = '\0';
        printf("message received: %s\n", buf);
    }
}
