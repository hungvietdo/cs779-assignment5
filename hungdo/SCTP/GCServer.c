/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc server.c -lpthread -o server
*/
 

#include "functions.h"
void Initialize();

//Group multicast info
char *MulticastIPAddress;
int UDP_Multicast_port; 

//Sockets
int socket_Q;
int socket_TCP;
int socket_UDP;
int UDP_Multicast_socket;

struct arg_struct {
    int client_sock;
    struct sockaddr_in from;
};

//Threads
pthread_t thread_id;
pthread_t thread_udp;
pthread_t thread_multicast;
pthread_t thread_q;


void addTCPClient(struct sockaddr_in cliaddr, int sockfd);
void addUclientList(struct sockaddr_in cliaddr);

void remove_tclient(int sockt);
void remove_uclient(struct sockaddr_in from);

void bind_UDP_Multicast_Socket(int port, char *MulticastIPAddress);

void sendMesgto_T_CLient_List(char mesg[MAXLINE]);
void sendMesgto_U_CLient_List(char buf[MAXLINE]);
void server_announcement(char clienttype[MAXLINE],struct sockaddr_in from);



socket_info UClient_List[FD_SETSIZE];
socket_info TClient_List[FD_SETSIZE];
int number_of_uclient, number_of_tclient;
char				mesg[MAXLINE],buf[MAXLINE];
int socket_desc , client_sock , n;
struct sockaddr_in server , client, servaddrTCP, servaddrUDP,from;
int 				rc;

//the thread function
void *TCPConnectionHandler(void *);
void *ServUDP(void *arg);
void *ServMulticast(void *arg);
void *ServQclient (void *arg);

int main(int argc , char *argv[])
{
	int fromlen,c;
	
	//struct sockaddr_in client;
	Initialize();
	
	//Create thread for udp
	pthread_create(&thread_udp,NULL,ServUDP,"foo");
	//Create thread for udp
	pthread_create(&thread_multicast,NULL,ServMulticast,"foo");

	//Create new thread for Q
	pthread_create(&thread_q,NULL,ServQclient,"foo");	
	
	c = sizeof(struct sockaddr_in);
	
	//Handling TCP client
	for (;;)
	    {
			fromlen = sizeof(client);
		if (client_sock = accept(socket_TCP, (struct sockaddr *)&client, (socklen_t*)&c))
			{
				
					
			 		//add new TCP client to the list
					addTCPClient(client,client_sock);
					
					struct arg_struct args;
					args.client_sock = client_sock;
					args.from = client;
					
					if( pthread_create( &thread_id , NULL ,  TCPConnectionHandler , (void*) &args) < 0)
			        {
			            perror("could not create thread");
			            return 1;
			        }
			
				}
	    }
 
//new thread for Q
pthread_join(thread_q,NULL);     	    

pthread_join(thread_multicast,NULL);     
//new thread for udp
pthread_join(thread_udp,NULL); 

return 0;
}

//serv Q
void *ServQclient (void *arg)
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

	// //Handling UDP client
	//fromlen = sizeof(from);
	
    for(;;){
		
	memset(buf, '\0', MAX_LEN);	

	 len = sizeof(struct sockaddr_in);
      rc = sctp_recvmsg(socket_Q, buf, sizeof(buf), (SA *)&cliaddr, &len, &sri, &msg_flags);
      buf[rc]= (char) NULL;
      printf("sctp_recvmsg: %s\n", buf);
          
 	  
 	  printf("sctp_recvmsg: %s\n", buf);

	   
	  //  //
	  //  addUclientList(from);
	   
	  
	  //  buf[rc]=NULL;
   //     if (rc > 0){
   //        	//	   
   //   		if (!strncmp(buf,"ByBy",4))
  	// 		{
  	// 			remove_uclient(from);
			// }
			// else if (!strncmp(buf,"newuclient",10))
	  // 			{
					
			// 		server_announcement("U",from);
			// 	}
			// else
			// {
			// 	char *sendbuf;
			// 	sendbuf = formatMesg("U",buf,from);
				
			// 	//printf("Unicast: %s\n", sendbuf);
				
			// 	//send to Unicast list
			// 	sendMesgto_U_CLient_List(sendbuf);
				
			// 	//to tcp list
			// 	sendMesgto_T_CLient_List(sendbuf);
				
			// 	//to UDP group
			// 	SendaMessagetoUDPGroup(sendbuf, UDP_Multicast_socket, MulticastIPAddress, UDP_Multicast_port);
			// }
	  //  }
	  //  memset(buf, 0, sizeof(buf));
    }
	return NULL;

}
 
//Unicast
void *ServUDP(void *arg)
{
	int fromlen;

	// //Handling UDP client
	fromlen = sizeof(from);
	
    for(;;){
		
	memset(buf, '\0', MAX_LEN);	
       fromlen = sizeof(from);
       if ((rc=recvfrom(socket_UDP, buf, sizeof(buf), 0, (SA *) &from, &fromlen)) < 0)
          perror("receiving datagram  message");
 	  
	   
	   //
	   addUclientList(from);
	   
	  
	   buf[rc]=NULL;
       if (rc > 0){
          	//	   
     		if (!strncmp(buf,"ByBy",4))
  			{
  				remove_uclient(from);
			}
			else if (!strncmp(buf,"newuclient",10))
	  			{
					
					server_announcement("U",from);
				}
			else
			{
				char *sendbuf;
				sendbuf = formatMesg("U",buf,from);
				
				//printf("Unicast: %s\n", sendbuf);
				
				//send to Unicast list
				sendMesgto_U_CLient_List(sendbuf);
				
				//to tcp list
				sendMesgto_T_CLient_List(sendbuf);
				
				//to UDP group
				SendaMessagetoUDPGroup(sendbuf, UDP_Multicast_socket, MulticastIPAddress, UDP_Multicast_port);
			}
	   }
	   memset(buf, 0, sizeof(buf));
    }
	return NULL;
}

//Multicast
void *ServMulticast(void *arg)
{
    char            recvBuf[MAX_LEN];
 	int fromlen,cc;
	struct sockaddr_in from;
	
	int myudpsendport;
 	struct hostent *he;
 	struct in_addr ipv4addr;
	char *MulticastIPAddress;
	int UDP_Multicast_port; 
	
	/*get my TCP port to */
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
	myudpsendport = 0;
    if (getsockname(UDP_Multicast_socket, (struct sockaddr *)&sin, &len) == -1)
        perror("getsockname");
    else
  	  myudpsendport = ntohs(sin.sin_port);
	
    for (;;) {
       
	    memset(recvBuf, '\0', MAX_LEN);

        /*bytes = recv(inSock, recvBuf, MAX_LEN, 0);*/
		fromlen= sizeof(from);
        if ((cc=recvfrom(UDP_Multicast_socket, recvBuf, sizeof(recvBuf), 0, (SA *) &from, &fromlen)) < 0)
			    perror("receiving echo");
     	
			/*check if UDP message is comming from server do nothing*/

			if ((myudpsendport != ntohs(from.sin_port))
			|| (strncmp (inet_ntoa(from.sin_addr),inet_ntoa(sin.sin_addr),sizeof(from.sin_addr)) !=0))
				{
					char *sendbuf;
					sendbuf = formatMesg("M",recvBuf,from);
					//printf("%s\n", sendbuf );
					
					//To TCP list
					sendMesgto_T_CLient_List(sendbuf);
					//send to Unicast list
					sendMesgto_U_CLient_List(sendbuf);

				}
		
		//	memset(sendbuf, 0, sizeof(sendbuf));
	 

	}
	return NULL;
}
//TCP
void *TCPConnectionHandler(void *socket_desc)
{
	
   
    int read_size, sock;
    char *message , client_message[2000];
	
	int checkmulticast = 0;
	char   recvBuf[MAX_LEN];
   	struct sockaddr_in from;
	
	struct arg_struct *args = socket_desc;
    //Get the socket descriptor
    sock = args->client_sock;
	from = args->from;
	//Receive a message from client
  //  
	//while (read_size = recvfrom(sock, client_message, sizeof(client_message), 0, (SA *) &from, &fromlen)>=0)
	
	while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
		
    {
		if (!strncmp(client_message,"IamaNewMulticastClient",22))
			{	
				checkmulticast =1;
				//Send Multicast info and exit
				char UDPServerInfo[MAXLINE];
				snprintf(UDPServerInfo, sizeof UDPServerInfo, "%s:%d", MulticastIPAddress,UDP_Multicast_port);
			   	if (send(client_sock, UDPServerInfo, strlen(UDPServerInfo), 0) <0 )
				    perror("sending stream message");
				
			}
		else if (!strncmp(client_message,"iamnewtcpclient", 15))
				{
						server_announcement("T",from);
				}
			else
				
			{

			char *sendbuf;
			sendbuf = formatMesg("T",client_message,from);
			
			//Send to Multicast group
			SendaMessagetoUDPGroup(sendbuf, UDP_Multicast_socket, MulticastIPAddress, UDP_Multicast_port);
			
			//Send to Unicast list
			sendMesgto_U_CLient_List(sendbuf);
			
			//send to TCP list
			sendMesgto_T_CLient_List(sendbuf);
			pthread_join( thread_id , NULL);		
		}
		//clear the message buffer
		memset(client_message, 0, 2000);
    }
     
    if(read_size == 0)
    {	
		//remove client out of the list
		remove_tclient(sock);
       // puts("Client disconnected");
        fflush(stdout);
		//this is Mclient
		if (checkmulticast==1)
			server_announcement("M",from);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
} 

//Message to TCP list
void sendMesgto_T_CLient_List(char mesg[MAXLINE])
{
	int i,sd;
	struct sockaddr_in cliaddr;
	
	cliaddr.sin_family = AF_INET;
	
	for (i=1;i<=number_of_tclient;i++)
		if (TClient_List[i].check==1)
			{	
				cliaddr = TClient_List[i].from;
				if (sendto(TClient_List[i].sockfd, mesg,strlen(mesg), 0, (SA *) &cliaddr, sizeof(cliaddr))< 0) 
		           printf("error in sendto %d\n",i);
			}
}
//Send message to U list
void sendMesgto_U_CLient_List(char buf[MAXLINE])
{
	int	sd;
    struct	sockaddr_in server;
    sd = socket (AF_INET,SOCK_DGRAM,0);
	server.sin_family = AF_INET;
   	int i;
	for (i=1;i<=number_of_uclient;i++)
	{
		if (UClient_List[i].check==1)
		{
			 server = UClient_List[i].from;
			 if (sendto(sd, buf,strlen(buf), 0, (SA *) &server, sizeof(server))< 0) {
 	            printf("error in sendto \n");
 	            exit(-1);
 	        }
		}
	}
	
	
}
//Add a TCP Client to the list
void addTCPClient(struct sockaddr_in cliaddr, int sockfd)
{
	int i;
	int inthelist = 0;
	
	for (i=1;i<=number_of_tclient;i++)
		if (TClient_List[i].check < 0)
			{
				break;
			}
			else
			{
				if (TClient_List[i].from.sin_port == cliaddr.sin_port)
				{
					printf("already in the list\n");
					inthelist = 1;
					break;
				}
			}
		
	//Client is not in the list	
	if (inthelist==0)
	{
		TClient_List[i].check =1;
		TClient_List[i].from = cliaddr;
		TClient_List[i].sockfd = sockfd;

		if (i>number_of_tclient) number_of_tclient++;

		if (i == FD_SETSIZE)
		{
			printf("too many TCP clients\n");
			exit(0);
		}
	}
	
	//printf("number_of_tclient: %d\n", number_of_tclient);
	
}
//Add a TCP Client to the list
void addUclientList(struct sockaddr_in cliaddr)
{
	int i;
	int inthelist = 0;
	
	for (i=1;i<=number_of_uclient;i++)
		if (UClient_List[i].check < 0)
			{
				break;
			}
			else
			{
				if ((UClient_List[i].from.sin_port == cliaddr.sin_port)
						|| (strncmp (inet_ntoa(UClient_List[i].from.sin_addr),inet_ntoa(cliaddr.sin_addr),sizeof(cliaddr.sin_addr)) !=0))
				{
					//printf("already in the list\n");
					inthelist = 1;
					break;
				}
			}
		
	//Client is not in the list	
	if (inthelist==0)
	{
		UClient_List[i].check =1;
		UClient_List[i].from = cliaddr;
		//UClient_List[i].sockfd = sockfd;

		if (i>number_of_uclient) number_of_uclient++;

		if (i == FD_SETSIZE)
		{
			printf("too many UDP clients\n");
			exit(0);
		}
	}
	//printf("number_of_uclient: %d\n", number_of_uclient);
}

void bind_UDP_Multicast_Socket(int port, char *MulticastIPAddress)
{

	
	struct sockaddr_in HostReceive;
  	HostReceive.sin_family = AF_INET;
	HostReceive.sin_port = port;
	HostReceive.sin_addr.s_addr = htonl(INADDR_ANY);

 	

	if ((UDP_Multicast_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("can't create UDP socket: \n");
		exit(-1);
	}
	reuseport(UDP_Multicast_socket);
	

	
	if (bind(UDP_Multicast_socket, (SA *) & HostReceive, sizeof(HostReceive)) < 0) {

		printf("error in bind1\n");
		exit(-1);
	}
	/** allow multicast datagrams to be transmitted to a distance
            according to the value put in "TimeToLive" variable */

	TimeToLive = 2;
	setttlvalue(UDP_Multicast_socket, &TimeToLive);

	loop = 1;		/* enable loopback */
	setloopback(UDP_Multicast_socket, loop);
	
	/*printf("%s:%d\n", MulticastIPAddress,port);*/
	
	joingroup(UDP_Multicast_socket, MulticastIPAddress);
	
}
//remove tclient out the list
void remove_tclient(int sock)
{
	int i;
 
	for (i=1;i<=number_of_tclient;i++)
	{
		if (TClient_List[i].sockfd == sock)
		{
				TClient_List[i].check=-1; //any client equal to sock will be deleted
				//break;
		}
	}
	
}
//remove tclient out the list
void remove_uclient(struct sockaddr_in from)
{
	int i;
 
	for (i=1;i<=number_of_uclient;i++)
	{
		if ((UClient_List[i].from.sin_port == from.sin_port)
				|| (strncmp (inet_ntoa(UClient_List[i].from.sin_addr),inet_ntoa(from.sin_addr),sizeof(from.sin_addr)) !=0))
		
		{
				UClient_List[i].check=-1; //any client equal to sock will be deleted
				//break;
		}
	}
	
}
void Initialize() {

	int i;
	for (i=0;i<=FD_SETSIZE;i++)
	{
		UClient_List[i].check=-1;
		TClient_List[i].check=-1;	
	}
	number_of_uclient = 0;
	number_of_tclient = 0;
	
	/*random IP address*/
	socket_info MultiCastInfo = randomUDPinfo();
	MulticastIPAddress = MultiCastInfo.ipaddress;
	UDP_Multicast_port = htons(MultiCastInfo.port);

	/*this port is using for receiving information*/
	printf ("Multicast IP: %s\n",MulticastIPAddress);
	printf ("Multicast Port: %d\n",UDP_Multicast_port);
	printf ("Server Port: %d\n",SERV_PORT);
	
	//To serve TCP server
	socket_TCP = socket(AF_INET, SOCK_STREAM, 0); //tcp server
	reuseport(socket_TCP);
	bzero(&servaddrTCP, sizeof(servaddrTCP));
	servaddrTCP.sin_family      = AF_INET;
	servaddrTCP.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddrTCP.sin_port        = htons(SERV_PORT);
	
	//To serve UDP server
	socket_UDP = socket(AF_INET, SOCK_DGRAM, 0); //udp server
	reuseport(socket_UDP);

	bzero(&servaddrUDP, sizeof(servaddrUDP));
	servaddrUDP.sin_family      = AF_INET;
	servaddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddrUDP.sin_port        = htons(SERV_PORT);

	//To serve Q server
	socket_Q = socket (AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
	reuseport(socket_Q);
	
	//This server use server TCP info
	bind( socket_Q, (SA *) &servaddrTCP, sizeof(servaddrTCP) );


	bind(socket_TCP, (SA *) &servaddrTCP, sizeof(servaddrTCP));
	bind(socket_UDP, (SA *) &servaddrUDP, sizeof(servaddrUDP));
	//bind multicast socket
	bind_UDP_Multicast_Socket(UDP_Multicast_port,MulticastIPAddress);

	listen(socket_Q, LISTENQ);
	listen(socket_TCP, LISTENQ);
	listen(socket_UDP, LISTENQ);
    
}


void server_announcement(char clienttype[MAXLINE],struct sockaddr_in from)
{
	char recvBuf[MAXLINE];
	
	//printf("%s\n", inet_ntoa(from.sin_addr));
	
	char *hostname ;//= getHostnamebyIP(inet_ntoa(from.sin_addr));
	
	hostname ="localhost";
	
	struct hostent *he;
 	struct in_addr ipv4addr;
	inet_pton(AF_INET, inet_ntoa(from.sin_addr), &ipv4addr);
	he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
	hostname = he->h_name;
	
	snprintf(recvBuf, sizeof(recvBuf), "New client (%s:%d)\n", hostname,ntohs(from.sin_port));

	if (!strncmp(clienttype,"U",1))
		snprintf(recvBuf, sizeof(recvBuf), "New (U) client (%s:%d)\n", hostname,ntohs(from.sin_port));

	if (!strncmp(clienttype,"T",1))
		snprintf(recvBuf, sizeof(recvBuf), "New (T) client (%s:%d)\n", hostname,ntohs(from.sin_port));

	if (!strncmp(clienttype,"M",1))
		snprintf(recvBuf, sizeof(recvBuf), "New (M) client (%s:%d)\n", hostname,ntohs(from.sin_port));

	//send to Unicast list
	sendMesgto_U_CLient_List(recvBuf);
	
	//to tcp list
	sendMesgto_T_CLient_List(recvBuf);
	
	//to UDP group
	SendaMessagetoUDPGroup(recvBuf, UDP_Multicast_socket, MulticastIPAddress, UDP_Multicast_port);
	
	printf("%s\n", recvBuf);
	
}

		//printf("TClient: %d , socket %d\n",ntohs(TClient_List[i].from.sin_port),TClient_List[i].sockfd);
		//printf("mesg to send %s\n", mesg );
		
