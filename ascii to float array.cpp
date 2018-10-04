#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atof */
#include <math.h>       /* sin */

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


//defines
#define URL_THINGSPEAK             "api.thingspeak.com"
#define PORT_THINGSPEAK            80
#define BEGIN_OF_HTTP_REQ          "GET /update?key="
#define END_OF_HTTP_REQ            "\r\n\r\n"
#define MAX_SIZE                   9999

#define THINGSPEAK_OFFLINE_ERROR     0
#define THINGSPEAK_CONNECTION_ERROR  1
#define OPEN_SOCKET_ERROR            2
#define PARAMS_ERROR                 3
#define SEND_OK                      4

char SendDataToThingSpeak(int FieldNo, float * FieldArray, char * Key, int SizeOfKey);

int main ()
{
    char buffer [20] = "12.34 20.35";
    float n;
    char data[10];
    long fdata;

    n = atof(buffer);
    printf("%f\n", n);

    for(int i=5; i<12;i++){
        data[i] = buffer[i];
        printf("%c", data[i]);
    }

    fdata = atof(data);
    printf("\n%l",fdata );
    return 0;
}

char SendDataToThingSpeak(int FieldNo, float * FieldArray, char * Key, int SizeOfKey)
{
	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *ServerTCP;
	int ReqStringSize;
	int i;
	char ReqString[MAX_SIZE];
	char BeginOfHTTPReq[]=BEGIN_OF_HTTP_REQ;
	char EndOfHTTPReq[]=END_OF_HTTP_REQ;
	char *ptReqString;

	if (FieldNo <=0)
		return PARAMS_ERROR;

	//Setting up HTTP Req. string:
	bzero(&ReqString,sizeof(ReqString));
	sprintf(ReqString,"%s%s",BeginOfHTTPReq,Key);

	ptReqString = &ReqString[0]+(int)strlen(ReqString);
	for(i=1; i<= FieldNo; i++)
	{
		sprintf(ptReqString,"&field%d=%.2f",i,FieldArray[i-1]);
		ptReqString = &ReqString[0]+(int)strlen(ReqString);
	}

	sprintf(ptReqString,"%s",EndOfHTTPReq);
	printf("%s",EndOfHTTPReq);
	//Connecting to ThingSpeak and sending data:
	portno = PORT_THINGSPEAK;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//Step 1: opening a socket
	if (sockfd < 0)
		return OPEN_SOCKET_ERROR;

	//Step 2: check if ThingSpeak is online
	ServerTCP = gethostbyname(URL_THINGSPEAK);
	if (ServerTCP == NULL)
	    return THINGSPEAK_OFFLINE_ERROR;

	//Step 3: setting up TCP/IP socket structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)ServerTCP->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         ServerTCP->h_length);
    serv_addr.sin_port = htons(portno);

	//Step 4: connecting to ThingSpeak server (via HTTP port / port no. 80)
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
		return THINGSPEAK_CONNECTION_ERROR;

	//Step 5: sending data to ThingSpeak's channel
    write(sockfd,ReqString,strlen(ReqString));

	//Step 6: close TCP connection
    close(sockfd);

	//All done!
	return SEND_OK;
}
