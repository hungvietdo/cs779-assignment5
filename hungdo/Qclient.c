#include "functions.h"
char *hostname[] = {"something","somethingelse","somethingmore","localhost"};

int Qsocket;
int port_to_connect;
char            sendBuf[MAX_LEN];
int             bytes = 0;
char buf[512];
int sd;
struct  sockaddr_in server;
struct  hostent *hp, *gethostbyname();

void senddata();
void getdata();

main(argc, argv )
int argc;
char *argv[];


{
    
    int i;

    port_to_connect = SERV_PORT;

    //port_to_connect = 10117;

    Qsocket = socket (AF_INET,SOCK_SEQPACKET,IPPROTO_SCTP);
    server.sin_family = AF_INET;
    

    for (i=0;i<4;i++)
    {


        printf("Trying to connect to server: %s\n", hostname[i]);


        hp = gethostbyname(hostname[i]);

        bcopy ( hp->h_addr, &(server.sin_addr.s_addr), hp->h_length);
    
        server.sin_port = htons(port_to_connect);
        
            if ( connect(Qsocket, (SA *) &server, sizeof(server)) < 0 ) {
                printf("connecting to server  failed\n");
            }
            else
            {
                printf("Successfully connected to %s\n", hostname[i]);
                break;
            }
        if (i==3)
        {
            printf("There is no server available\n");
            return 0;
        }
    }


    pthread_t sendmesg_thread;
    pthread_t getmesg_thread;
    
    
    pthread_create(&sendmesg_thread,NULL,senddata,"foo");
    //Create thread for udp
    pthread_create(&getmesg_thread,NULL,getdata,"foo");
    
    pthread_join(sendmesg_thread,NULL);     
    //new thread for udp
    pthread_join(getmesg_thread,NULL); 

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
            else if (sendto(Qsocket, sendBuf, bytes, 0, (SA *) & server, sizeof(server)) < 0) 
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
        rc=recv (Qsocket, buf, sizeof(buf), 0);
        buf[rc]= (char) NULL;
        printf("Received: %s", buf);
    }
    wait(NULL);
    
}   