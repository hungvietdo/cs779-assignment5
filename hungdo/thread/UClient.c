#include "functions.h"


int	UDPsocket;
int parentprocess = 0;
char buf[512];
struct	sockaddr_in server,waitserver;
char *hostname[] = {"something","somethingelse","somethingmore","localhost"};
struct  hostent *hp, *gethostbyname();
char            sendBuf[MAX_LEN];
int 			flag = 0;
int 			breakflag = 0;
int             bytes = 0;

void senddata();
void getdata();

void sig_handler(int signo)
{
	
  if (signo == SIGINT)
  	{
		if (parentprocess !=0)
		{
  		printf("\nI am leaving.\n");
		
		if (sendto(UDPsocket, "ByBy", 4, 0, (SA *) & server, sizeof(server)) < 0) 
			{
	            printf("error in sendto \n");
	            exit(-1);
		    }
		}
	}
	/*Exit program*/
	exit(0);
}


void handler_alarm(int sig)
{
	
    signal(SIGALRM, handler_alarm);

	if (breakflag>2) 
		{
			printf("Can not connect to UMserver: %s\n",hostname[breakflag-1]);
			exit(0);
		}
	if (breakflag>0)
	  printf("Can not connect to UMserver: %s\n",hostname[breakflag-1]);
		
	if ((hp = gethostbyname(hostname[breakflag])) == NULL)
	{
		printf("Error when getting hostname %s\n", hostname[breakflag] );
	}
	else
	{
		bcopy ( hp->h_addr, &(server.sin_addr.s_addr), hp->h_length);
			if (sendto(UDPsocket, "newuclient",10 , 0, (SA *) & server, sizeof(server)) < 0)
				{
	            	printf("error in sendto \n");
	            	exit(-1);
	        	}
	}
	
	breakflag++;
	flag=1;
	alarm(2);
}

 
 
int main(int argc, char *argv[])
{
	 
	
	
	
	srand(time(NULL));
	int randport=(rand() % 1000) + 10000;
	
	waitserver.sin_family = AF_INET; 
	waitserver.sin_addr.s_addr = htonl(INADDR_ANY);
	
	waitserver.sin_port = htons(randport);  
	UDPsocket = socket (AF_INET,SOCK_DGRAM,0); 
	reuseport(UDPsocket);
	
    if ( bind( UDPsocket, (SA *) &waitserver, sizeof(waitserver) ) < 0 ) {
         close(UDPsocket);
         perror("binding name to datagram socket");
         exit(-1);
      } 
	 
   /** get port information and  prints it out */
   int length = sizeof(waitserver);
      if ( getsockname (UDPsocket, (SA *)&waitserver,&length) ) {
         perror("getting socket name");
         exit(0);
      }
    
	printf("...My UDP Port is: %d...\n", ntohs(waitserver.sin_port)); 

	server.sin_family = AF_INET;
	server.sin_port = htons(SERV_PORT);
	
	/*Catching sigint when exit*/
   // if (signal(SIGINT, sig_handler) == SIG_ERR)
    //	printf("\ncan't catch SIGINT\n");
	
	signal(SIGALRM, handler_alarm);
	alarm(2);
	while(1){
		int rc = 0;
		
		sleep(1);
		rc=recv (UDPsocket, buf, sizeof(buf), 0);
		
	    if (flag==1)
		{
			buf[rc]= (char) NULL;
			printf("ChatServer hostname is:%s \n\n",hostname[breakflag-1]);
			alarm(0);
			break;
	   }
	}
	
	
	// if (fork() == 0)
// 	{
//
// 		senddata();
//   	}
// 	else
// 	{
// 		getdata();
// 	}
		
	pthread_t sendmesg_thread;
	pthread_t getmesg_thread;
	
	
	pthread_create(&sendmesg_thread,NULL,senddata,"foo");
	//Create thread for udp
	pthread_create(&getmesg_thread,NULL,getdata,"foo");
	
 	pthread_join(sendmesg_thread,NULL);     
	//new thread for udp
	pthread_join(getmesg_thread,NULL); 

return 0;			
				
		
}

void senddata()
{
	//parentprocess = getppid();
    
	printf("type in message to be sent to the group followed by ENTER\n");
    printf("hit CTRL-C to quit from the group \n");
	
	

    for (;;) 
		{
			/*clean sendBuf*/
	        memset(sendBuf, '\0', MAX_LEN);
	        bytes = read(0, sendBuf, MAX_LEN);
	        if (bytes < 0) 
				{
		            printf("error in reading from STDIN \n");
		            exit(-1);
				} 
			else if (sendto(UDPsocket, sendBuf, bytes, 0, (SA *) & server, sizeof(server)) < 0) 
				{
		            printf("error in sendto \n");
		            exit(-1);
		        }
		}
}
	
void getdata()
{
 	sleep(1);
	int rc;
    for(;;) 
	{
        rc=recv (UDPsocket, buf, sizeof(buf), 0);
        buf[rc]= (char) NULL;
        printf("Received: %s", buf);
	}
	wait(NULL);
	
}	