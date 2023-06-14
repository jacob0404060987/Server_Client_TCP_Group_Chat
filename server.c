#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


struct client_info {
	int sockno;
	char ip[INET_ADDRSTRLEN];
};


int clients[100];
int m=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void sendMessage(char* message, int numb)
{

    int i;
    pthread_mutex_lock(&mutex);
    for(i=0;i<m;i++)
{
    if(clients[i]!=numb)
    {
       if(send(clients[i],message,strlen(message),0)<0)
        {
            perror("sending failure...");
				continue;
        }
    }
}
    pthread_mutex_unlock(&mutex);

}


void* receiceMessage(void* socket)
{
    struct client_info clinet = *((struct client_info*)socket);
    char message[500];
    int lenght;
    int i,j;

    while ((lenght = recv(clinet.sockno, message, 500, 0))>0)
    {
        message[lenght]='\0';
        sendMessage(message,clinet.sockno);
        memset(message,'\0',sizeof(message));
    }
    pthread_mutex_lock(&mutex);
    printf("%s disconnected...\n",clinet.ip);

    for (i = 0; i < m; i++) 
	{
		if (clients[i] == clinet.sockno) 
		{
			j = i;
		
			while (j < (m - 1)) 
			{
				clients[j] = clients[j + 1];
				j++;
			}
		}
	}

	m--;
    pthread_mutex_unlock(&mutex);
    

}
/*
void something(int newsockfd)
{
char buffer[256];
int n;
while(1){
bzero(buffer,256);
 n = read(newsockfd,buffer,255); // read from connection socket
 if (n < 0) error("ERROR reading from socket"); // if error
 printf("Here is the message: %s\n",buffer); // show the message
 n = write(newsockfd,"I got your message",18); // notify
 if (n < 0) error("ERROR writing to socket"); //
} 
}
*/
int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr, cli_addr;
    struct client_info cl;
    socklen_t their_addres_size;
    int sockfd, newsockfd, portno, clilen;
    pthread_t sendt, recvt;
    char msg[500];
    char ip[INET_ADDRSTRLEN];
    if(argc<2)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
     portno = atoi(argv[1]); // get port number
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // makes a socket
 if (sockfd < 0) 
 {
 error("ERROR opening socket"); // if socket creation failed
 }
 memset(serv_addr.sin_zero,'\0', sizeof(serv_addr.sin_zero));
 serv_addr.sin_family = AF_INET; // specify address family (IPv4)
  serv_addr.sin_port = htons(portno); // set port number
 serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // bind IP addresses
 their_addres_size = sizeof(cli_addr);
 if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
 {
 error("ERROR on binding"); // error while binding socket
 }
 listen(sockfd,5); // listen on created socket
 while (1)
 {
    newsockfd = accept(sockfd, (struct sockaddr *) & cli_addr, &their_addres_size);
    pthread_mutex_lock(&mutex);
    inet_ntop(AF_INET,(struct sockaddr*) & cli_addr, ip, INET_ADDRSTRLEN);
    printf("%s connected...\n",ip);
    cl.sockno = newsockfd;
    strcpy(cl.ip,ip);
    clients[m]=newsockfd;
    m++;

    pthread_create(&recvt, NULL, receiceMessage, &cl);
		pthread_mutex_unlock(&mutex);
 }
  

return 0;
}
    