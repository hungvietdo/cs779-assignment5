/*
NAME:           TCPCleint1 
SYNOPSIS:       TCPCleint1 <hostid> <portnumber> 
DESCRIPTION:    The program creates a stream socket in the inet domain, 
                Connect to TCPServer1, 
                Get messages typed by a user and 
                Send them to TCPServer1 running on hostid
                Then it waits for a reply from  the TCPServer1
                and show it back to the user, with a message
                indicating if there is an error during the round trip. 
*/

#include "functions.h"
#define MAXHOSTNAME 80
#define BUFSIZE 1024
char buf[BUFSIZE];
char rbuf[BUFSIZE];
void GetUserInput();
void readdata();
  int rc, cc;
  int   sd;

main( argc, argv )
int argc;
char *argv[];
{
  int childpid;
  struct   sockaddr_in server;
  struct   sockaddr_in client;
  struct  hostent *hp, *gethostbyname();
  struct  servent *sp;
  struct sockaddr_in from;
  struct sockaddr_in addr;
  int fromlen;
  int length;
  char ThisHost[80];
  int tcpPortAssigned;

  sp = getservbyname("echo", "tcp");
  
  char *hostname[] = {"something","somethingelse","somethingmore","localhost"};
  int hostnameid;
  
for (hostnameid=0;hostnameid<4;hostnameid++)
 {
		printf("Trying to connect to %s\n", hostname[hostnameid]);
		sleep(1);

		if ((hp = gethostbyname(hostname[hostnameid])) == NULL)
		{
			printf("Error when getting hostname %s\n", hostname[hostnameid] );
		}
		else

		{
			sd = socket (AF_INET,SOCK_STREAM,0);
			server.sin_family = AF_INET;
		    bcopy ( hp->h_addr, &(server.sin_addr.s_addr), hp->h_length);
		    server.sin_port = htons(SERV_PORT);
			if (connect(sd, (SA *) &server, sizeof(server))<0) {
		        perror("connect error");
		        close(sd);
		    }
		 	else
		 	{
		 		printf("Connect successfully to %s\n",hostname[hostnameid] );
		 		break;
		 	}
		}
	}
	
	
	/*get my TCP port to */
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(sd, (struct sockaddr *)&sin, &len) == -1)
        perror("getsockname");
    else
  	  tcpPortAssigned = sin.sin_port;
	
	printf("%d\n", ntohs(tcpPortAssigned) );
	

  /* Construct name of socket to send to. */
    server.sin_family = AF_INET; 
  /* OR server.sin_family = hp->h_addrtype; */

  server.sin_port = htons(SERV_PORT);
  
  if (send(sd, "iamnewtcpclient", 15, 0) <0 )
	{
    	perror("sending stream message");
  
	}
  
	pthread_t sendmesg_thread;
	pthread_t getmesg_thread;
	

	pthread_create(&sendmesg_thread,NULL,GetUserInput,"foo");
	//Create thread for udp
	pthread_create(&getmesg_thread,NULL,readdata,"foo");
	
 	pthread_join(sendmesg_thread,NULL);     
	//new thread for udp
	pthread_join(getmesg_thread,NULL); 
 
}

cleanup(buf)
char *buf;
{
  int i;
  for(i=0; i<BUFSIZE; i++) buf[i]=NULL;
}

void readdata()
{
    for(;;) {
      cleanup(rbuf);
      if( (rc=recv(sd, rbuf, sizeof(buf), 0)) < 0){
        perror("receiving stream  message");
        exit(-1);
      }
      if (rc > 0){
        rbuf[rc]=NULL;
        printf("Received: %s", rbuf);
      }else {
        printf("Disconnected..\n");
        close (sd);
        exit(0);
      }

    }
	
}

void GetUserInput()
{
printf("\nType anything followed by RETURN, or type CTRL-D to exit\n");
for(;;) {
    
    cleanup(buf);
    rc=read(0,buf, sizeof(buf));
    if (rc == 0) break;
    if (send(sd, buf, rc, 0) <0 )
	{
      perror("sending stream message");
	  break;
  	}
  }
  printf ("EOF... exit\n");
  close(sd);
  kill(getppid(), 9);
  exit (0);
}
 
   	

	 
