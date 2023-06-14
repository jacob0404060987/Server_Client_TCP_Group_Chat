#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
// define function that deals with errors
void* receiveMessage(void* socket)
{
  int newsock=*((int*)socket);
  char message[500];
  int lenght;

  while ((lenght=recv(newsock, message, 500,0))>0)
  {
    message[lenght]='\0';
    fputs(message,stdout);
    memset(message,'\0',sizeof(message));
  }
  
  
}

int main(int argc, char *argv[])
{
struct sockaddr_in cli_addr; // define structs
 int sockfd,newsock, portno,lenght,cli_addr_size; // define ints
  char message[500], username[20], res[500], ip[INET_ADDRSTRLEN];
 pthread_t sendt, recvt;
 if (argc < 3) {
 fprintf(stderr,"usage %s hostname port\n", argv[0]);
 exit(0); // exit main() function
 }
 portno = atoi(argv[2]); // get port number
 strcpy(username,argv[1]);
 sockfd = socket(AF_INET, SOCK_STREAM, 0); // create socket
 if (sockfd < 0)
 {
 error("ERROR opening socket"); // if socket creation failed
 }
 memset(cli_addr.sin_zero,'\0',sizeof(cli_addr.sin_zero));
 cli_addr.sin_family = AF_INET;
cli_addr.sin_port = htons(portno);
cli_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
connect(sockfd,(struct sockaddr*) & cli_addr, sizeof(cli_addr));
if(inet_ntop(AF_INET,(struct sockaddr*) & cli_addr,ip,INET_ADDRSTRLEN)<0)
{
  printf("DUPADUPADUAP");
}
	printf("connected to %s, start chatting\n", ip);
pthread_create(&recvt, NULL, receiveMessage, &sockfd);
while (fgets(message, 500, stdin) > 0) 
	{
		strcpy(res, username);
		strcat(res, ": ");
		strcat(res, message);
	
		lenght = write(sockfd, res, strlen(res));
		
		if (lenght < 0) 
		{
			perror("message not sent...");
			exit(1);
		}

		memset(message, '\0', sizeof(message));
		memset(res, '\0', sizeof(res));
	}
 
  close(sockfd); // close socket
  pthread_join(recvt, NULL);

return 0; // exit main() function
}