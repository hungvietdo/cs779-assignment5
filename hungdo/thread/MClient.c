/*
 * NAME:           mcastChat SYNOPSIS:       mchat <mulicast IP> <port>
 * DESCRIPTION:    The program creates a udp socket  bind it to <port> and
 * joins <multicast IP> it read input from stdin and send it to socket  and
 * read chat data from socket and send it to stdout Example: mcastChat
 * 224.0.0.1 10123
 */

#include "functions.h"
 

#define SA      struct sockaddr
#define MAXHOSTNAME 80
#define BUFSIZE 1024
#define MAX_LEN 1024

void sendMessage1();
void getMessage1();


int             UDPport;
char           *MulticastIPAddress;
int             UDPsocket, UDPsocketSend;
struct sockaddr_in LocalHost, HostReceive, HostSend;

char *hostname[] = {"something","somethingelse","somethingmore","localhost"};
int ccc;

 
int main(int argc, char *argv[])
{
	int tcpPortAssigned;
    
    
	char *v_Multicast;
	/*
		Connect to TCP server to get UDP info
	*/
    int	sd;
    struct	sockaddr_in server;
    struct  hostent *hp, *gethostbyname();
    int rc;
	char rbuf[MAX_LEN];
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
			    server.sin_port = htons(10378);
	    
				if (connect(sd, (SA *) &server, sizeof(server))<0) {
			        perror("connect error");
			        close(sd);
			    }
			 	else
			 	{
			 		printf("Connect successfully to %s\n",hostname[hostnameid] );
				   	if (send(sd, "IamaNewMulticastClient\n", 23, 0) <0 )
					    perror("sending stream message");
					
			 		break;
			 	}
		
			}
		}	
	
    if( (rc=recv(sd, rbuf, sizeof(rbuf), 0)) < 0)
		{
	      perror("receiving stream  message");
	      exit(-1);
	    }
    if (rc > 0)
		{
	      rbuf[rc]=NULL;
		  v_Multicast = rbuf;
	      
	    } 
	
	 
	
	/*get my TCP port to */
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(sd, (struct sockaddr *)&sin, &len) == -1)
        perror("getsockname");
    else
  	  tcpPortAssigned = ntohs(sin.sin_port);
  
  	
	close(sd);
    /*
    	Translate UDP server information
    */
	char *ch, *ch1;
	ch = strtok(v_Multicast, ":"); 
	ch1 = strtok(NULL, ":");
	
	
	
	MulticastIPAddress = ch;
    UDPport = atoi(ch1);
	
	printf("%s:%d\n", MulticastIPAddress,UDPport);
	
    HostReceive.sin_family = AF_INET;
	HostReceive.sin_port = UDPport;
	HostReceive.sin_addr.s_addr = htonl(INADDR_ANY);

	HostSend.sin_family = AF_INET;
/*	sending UDP message using socket on TCP port*/
	HostSend.sin_port = ntohs(tcpPortAssigned); 
	HostSend.sin_addr.s_addr = htonl(INADDR_ANY);
	
	printf("My TCP port is %d\n", tcpPortAssigned);
	printf("Chatserver is %s\n", hostname[hostnameid]);
	

	if ((UDPsocketSend = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("can't create UDP socket: \n");
		exit(-1);
	}
	
	if ((UDPsocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("can't create UDP socket: \n");
		exit(-1);
	}
	
	reuseport(UDPsocket);

	if (bind(UDPsocket, (SA *) & HostReceive, sizeof(HostReceive)) < 0) {

		printf("error in bind1\n");
		exit(-1);
	}
	/** allow multicast datagrams to be transmitted to a distance
            according to the value put in "TimeToLive" variable */

	TimeToLive = 2;
	setttlvalue(UDPsocket, &TimeToLive);

	loop = 1;		/* enable loopback */
	setloopback(UDPsocket, loop);

	joingroup(UDPsocket, MulticastIPAddress);
	
	pthread_t sendmesg_thread;
	pthread_t getmesg_thread;
	
	
	pthread_create(&sendmesg_thread,NULL,sendMessage1,"foo");
	//Create thread for udp
	pthread_create(&getmesg_thread,NULL,getMessage1,"foo");
	
 	pthread_join(sendmesg_thread,NULL);     
	//new thread for udp
	pthread_join(getmesg_thread,NULL); 

}

void sendMessage1()
{
    char            sendBuf[MAX_LEN];
    int             bytes = 0;
    struct sockaddr_in GroupAddress;
	
	int inSock = 0;
	int outSock = UDPsocketSend;

	reuseport(outSock);
	if (bind(outSock, (SA *) & HostSend, sizeof(HostSend)) < 0) {

		printf("error in bind\n");
		exit(-1);
	}
	
    GroupAddress.sin_family = AF_INET;
    GroupAddress.sin_port = UDPport;
    GroupAddress.sin_addr.s_addr = inet_addr(MulticastIPAddress);
	

    printf("type in message to be sent to the group followed by ENTER\n");
    printf("hit CTRL-D to quit from the group \n");

    for (;;) {
        memset(sendBuf, '\0', MAX_LEN);

        bytes = read(inSock, sendBuf, MAX_LEN);

        if (bytes < 0) {
            printf("error in reading from STDIN \n");
            exit(-1);
        } else if (bytes == 0) {
            kill(getppid(), 9);
            exit(0);
        } else if (sendto(outSock, sendBuf, bytes, 0, (SA *) & GroupAddress, sizeof(GroupAddress)) < 0) {
            printf("error in sendto \n");
            exit(-1);
        }
    }
}

/*
 * Get the multicast message sent by the group and print out to STDOUT
 */

void getMessage1()
{
    int             bytes = 0;
    char            recvBuf[MAX_LEN];
	int fromlen,cc;
 	struct sockaddr_in from;
 
	int inSock = UDPsocket;
	int outSock =1;
    for (;;) {
        memset(recvBuf, '\0', MAX_LEN);

		fromlen= sizeof(from);
        if ((bytes=recvfrom(inSock, recvBuf, sizeof(recvBuf), 0, (SA *) &from, &fromlen)) < 0)
            printf("error in reading from multicast socket\n");
        
		recvBuf[bytes]=NULL;
		
        //bytes = recv(inSock, recvBuf, MAX_LEN, 0);
        
		if (bytes < 0) {
            printf("error in reading from multicast socket\n");
            exit(-1);
        }
        /*
                        else if (bytes == 0)
        			printf("zero bytes read\n");
        */
		if ((!strncmp(recvBuf,"(U)",3)) ||  (!strncmp(recvBuf,"(T)",3)) || (!strncmp(recvBuf,"New",3)))
			
			{printf("Received: %s",recvBuf);}
		else
			{
				char *sendbuf;
				sendbuf = formatMesg("M",recvBuf,from);
				printf("Received: %s",sendbuf);
			}
		
        // else {		/* print the message to STDOUT */
    //         if (write(outSock, recvBuf, bytes) < 0) {
    //             printf("error in write to STDOUT \n");
    //             exit(-1);
    //         }
    //     }
    }
}
