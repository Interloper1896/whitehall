#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

struct _client
{
	char ipAddress[40];
	int port;
	char name[40];
} tcpClients[4];
int nbClients;
int fsmServer;


void printClients()
{
	int i;

	for (i=0;i<nbClients;i++)
		printf("%d: %s %5.5d %s\n",i,tcpClients[i].ipAddress, 
			tcpClients[i].port,
			tcpClients[i].name);
}

int findClientByName(char *name)
{
	int i;

	for (i=0;i<nbClients;i++)
		if (strcmp(tcpClients[i].name,name)==0)
			return i;
	return -1;
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void sendMessageToClient(char *clientip, int clientport, char *mess)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server = gethostbyname(clientip);

    if (server == NULL) 
	{
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(clientport);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("ERROR connecting\n");
        exit(1);
    }

    sprintf(buffer,"%s\n",mess);
    n = write(sockfd,buffer,strlen(buffer));

    close(sockfd);
}

void broadcastMessage(char *mess)
{
	int i;

	for (i=0;i<nbClients;i++)
		sendMessageToClient(tcpClients[i].ipAddress, 
			tcpClients[i].port,
			mess);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    char reply[256];
	char com;
	char clientIpAddress[256], clientName[256];
	int clientPort;
	int id;
	int position;
	int tour = 0;

    struct sockaddr_in serv_addr, cli_addr;
    int n;


	nbClients=0;
	fsmServer=0;

    if (argc < 2) 
	{
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
       error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    while (1)
    {    
    	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    	
		if (newsockfd < 0) 
         	error("ERROR on accept");
    	bzero(buffer,256);
     	n = read(newsockfd,buffer,255);

     	if (n < 0) 
			error("ERROR reading from socket");

    	printf("Received packet from %s:%d\nData: [%s]\n\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);

		printf("buffer[0]=%c\n",buffer[0]);

		switch (buffer[0])
		{
		case 'C':
			sscanf(buffer,"%c %s %d %s", &com, clientIpAddress, &clientPort, clientName);
			printf("COM=%c ipAddress=%s port=%d name=%s\n", com, clientIpAddress, clientPort, clientName);
			
			// fsmServer==0 alors j'attends les connexions de tous les joueurs
			if (fsmServer==0)
			{
				strcpy(tcpClients[nbClients].ipAddress,clientIpAddress);
				tcpClients[nbClients].port=clientPort;
				strcpy(tcpClients[nbClients].name,clientName);
				nbClients++;

				printClients();

				id=findClientByName(clientName);
				printf("id=%d\n",id);
				sprintf(reply,"I %d",id);
				sendMessageToClient(tcpClients[id].ipAddress, tcpClients[id].port, reply);	

				broadcastMessage("T messagesent");
				if (nbClients==4)
				{
					fsmServer=1;
					broadcastMessage("T Tout_le_monde_est_connecte,_le_jeu_peut_commencer_!");
					sprintf(reply,"O %d", tour);
					broadcastMessage(reply);
					sprintf(reply,"T Au_joueur_%s_de_jouer_!!", tcpClients[tour].name);
					broadcastMessage(reply);
				}
			}
			break;
		case 'X':
			sscanf(buffer,"%c %d %d", &com, &id, &position);
			printf("COM=%c id=%d position=%d\n", com, id, position);
			if (fsmServer == 1)
			{
				if (id == tour)
				{
					if (id != 0)  // si un policier se déplace, les autres policiers sont avertis de ce déplacement
					{
						sprintf(buffer, "P %d %d", position, id);
						broadcastMessage(buffer);	
					}
					tour++;
					sprintf(reply,"O %d", tour);
					broadcastMessage(reply);
					sprintf(reply,"T Au_joueur_%s_de_jouer_!!", tcpClients[tour].name);
					broadcastMessage(reply);
				}
			}
			break;
		default:
			break;
		}

		if (tour == 4) tour = 0;	
	
    	close(newsockfd);
    }
    close(sockfd);
    return 0; 
}
