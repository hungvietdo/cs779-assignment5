#include "functions.h"

socket_info randomUDPinfo()
{
	
/*random multicast IP and port*/
	char buf[256];
	char buf1[256];
	srand(time(NULL));
	int r1 = (rand() % (239-224)) + 224;
	int r2 = rand() % 255;
	int r3 = rand() % 255;
	int r4 = rand() % 255;
	int rport = (rand() % 1000) + 10000;

snprintf(buf, sizeof buf, "%d.%d.%d.%d", r1, r2, r3, r4);
snprintf(buf1, sizeof buf1, "%d", rport);
puts(buf);
puts(buf1);

socket_info mip;

mip.port = rport;

mip.ipaddress = malloc(strlen(buf)+1);
strcpy(mip.ipaddress,buf);

return mip;
}


void reuseport(int s)
{
	
	int one = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *) &one, sizeof(one));
	#ifdef __APPLE__
	setsockopt(s, SOL_SOCKET, SO_REUSEPORT, (const char *) &one, sizeof(one));
	#endif
	
}

/*Join group UDP*/
void joingroup(int s, char *group)
{
    struct sockaddr_in groupStruct;
    struct ip_mreq  mreq;	/* multicast group info structure */

    if ((groupStruct.sin_addr.s_addr = inet_addr(group)) == -1)
        printf("error in inet_addr\n");

    /* check if group address is indeed a Class D address */
    mreq.imr_multiaddr = groupStruct.sin_addr;
    mreq.imr_interface.s_addr = INADDR_ANY;

    if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreq,
                   sizeof(mreq)) == -1) {
        printf("error in joining group \n");
        exit(-1);
    }
}


/*
 * This function sets the Time-To-Live value
 */

void setttlvalue(int s, u_char * ttl_value)
{
    if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, (char *) ttl_value,
                   sizeof(u_char)) == -1) {
        printf("error in setting loopback value\n");
    }
}


/*
 * By default, messages sent to the multicast group are looped back to the local
 * host. this function disables that. loop = 1 /* means enable loopback loop
 * = 0 /* means disable loopback NOTE : by default, loopback is enabled */

void setloopback(int s, u_char loop)
{
    if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, (char *) &loop,
                   sizeof(u_char)) == -1) {
        printf("error in disabling loopback\n");
    }
}


void
SendaMessagetoUDPGroup(char *recvBuf, int outSock,  char *MulticastIPAddress, int UDPport)
{
    char            sendBuf[MAX_LEN];
    int             bytes = 0;
    struct sockaddr_in GroupAddress;

	GroupAddress.sin_family = AF_INET;
    GroupAddress.sin_port = UDPport;
    GroupAddress.sin_addr.s_addr = inet_addr(MulticastIPAddress);

    if (sendto(outSock, recvBuf, strlen(recvBuf), 0, (SA *) & GroupAddress, sizeof(GroupAddress)) < 0) {
            printf("error in sendto \n");
            exit(-1);
        }
    
}

char *getHostnamebyIP(char *ipaddress)
{   struct in_addr ip;
    struct hostent *hp;
	//parse ip
	if (!inet_aton(ipaddress, &ip))
            errx(1, "can't parse IP address %s", ipaddress);
	hp = gethostbyaddr((const void *)&ip,sizeof ip, AF_INET);
	return hp->h_name;
}
char *formatMesg(char clienttype[MAXLINE],char Mesg[MAXLINE],struct sockaddr_in from)
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
	
	snprintf(recvBuf, sizeof(recvBuf), "(%s:%d): %s", hostname,ntohs(from.sin_port),Mesg);
	
	if (!strncmp(clienttype,"U",1))
		snprintf(recvBuf, sizeof(recvBuf), "(U) (%s:%d): %s", hostname,ntohs(from.sin_port),Mesg);
	
	if (!strncmp(clienttype,"T",1))
		snprintf(recvBuf, sizeof(recvBuf), "(T) (%s:%d): %s", hostname,ntohs(from.sin_port),Mesg);
	
	if (!strncmp(clienttype,"M",1))
		snprintf(recvBuf, sizeof(recvBuf), "(M) (%s:%d): %s", hostname,ntohs(from.sin_port),Mesg);
	return recvBuf;
	
}

