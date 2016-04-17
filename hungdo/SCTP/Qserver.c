/*
NAME:        udpServerSctp 
SYNOPSIS:    udpServerSctp 
DESCRIPTION:    The program creates a datagram socket in the inet 
                domain and using SEQPACKET of SCTP protocol, 
                binds it to port <port> and receives any message
                arrived to the socket and prints it out
*/

void reusePort(int s);

#include "functions.h"

main(int argc, char* argv[])
{
   int   sd;
   struct   sockaddr_in server;
   char buf[512];
   int rc;
   int msg_flags;
   struct sockaddr_in cliaddr;
   int len;
   struct sctp_sndrcvinfo sri;
   struct sctp_event_subscribe evnts;
   int stream_increment=1;
   struct sctp_initmsg initm;


   server.sin_family = AF_INET;
   server.sin_addr.s_addr = htonl(INADDR_ANY);
   server.sin_port = htons(atoi(argv[1]));

   sd = socket (AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
   reusePort(sd);

   if (   bind( sd, (SA *) &server, sizeof(server) ) < 0 ){
      printf("error binding\n");
      exit(-1) ;
   }

   listen(sd, 1);

   for(;;){
      len = sizeof(struct sockaddr_in);
      rc = sctp_recvmsg(sd, buf, sizeof(buf), (SA *)&cliaddr, &len, &sri, &msg_flags);
      buf[rc]= (char) NULL;
      printf("sctp_recvmsg: %s\n", buf);

      rc = recvfrom (sd, buf, sizeof(buf), 0, (SA *) &cliaddr, &len);
      buf[rc]= (char) NULL;
      printf("recvfrom: %s\n", buf);

      len = sizeof(cliaddr);
      getpeername(sd, (SA *) &cliaddr, &len);
      printf("remote addr: %s,  port %d\n",
          inet_ntop(AF_INET, &cliaddr.sin_addr, buf,
          sizeof(buf)), ntohs(cliaddr.sin_port));

   }
}
void reusePort(int s)
{
  int one=1;

  if ( setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(char *) &one,sizeof(one)) == -1 )
  {
    printf("error in setsockopt,SO_REUSEPORT \n");
    exit(-1);
  }
}

