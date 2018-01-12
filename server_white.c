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
	int position, posJack;
	int tour = 0;
	int jackEstPasse[533] = { 0 };
	int zoneDeDecouverte[533] = { 0 };
	int indiceOuZone; // necessaire lors de la demande par click gauche
	int nbZonesDecouvertesPolice = 0;
	int nbZonesDecouvertesJack = 0;
	int compteursTotJack = 0;

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

	int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;   // flags de l'initialisation

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
		case 'C':   // phase de connection. Si réussite, passage à la phase d'initialisation
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
					broadcastMessage("T Tout_le_monde_est_connecte_!");
					printf("Phase d'initialisation\n");

					sprintf(reply,"J %d", fsmServer);
					broadcastMessage(reply);
				}
			}
			break;
		case 'X':     // changement de position d'un joueur
			sscanf(buffer,"%c %d %d", &com, &id, &position);
			printf("COM=%c id=%d position=%d\n", com, id, position);
			if (fsmServer == 2)
			{
				if (id == tour)
				{
					printf("prise en compte du tour\n");
					if (id != 0)  // si un policier se déplace, les autres policiers sont avertis de ce déplacement
					{
						sprintf(buffer, "P %d %d", position, id);
						broadcastMessage(buffer);	
						printf("envoi de la position aux autres policiers\n");
					}
					else
					{
						compteursTotJack++;
						sprintf(reply,"T Tours_de_Jack=%d/16", compteursTotJack);
						broadcastMessage(reply);
						jackEstPasse[position] = 1;
						posJack = position;
						printf("position Jack ajoutée\n");
						// si Jack atteind une zone de découverte
						if (zoneDeDecouverte[position] == 1)
						{
							compteursTotJack = 0;
							nbZonesDecouvertesJack++;
							sprintf(reply,"T Jack_a_atteind_une_zone_de_decouverte_!!");
							broadcastMessage(reply);
							sprintf(buffer, "Z %d", position);
							broadcastMessage(buffer);	
							printf("envoi de la zone de decouverte aux policiers\n");
							zoneDeDecouverte[position] = 0;
							// Si Jack a atteind toutes ses zones de decouverte
							if (nbZonesDecouvertesJack == 4)
							{
								sprintf(reply,"T JEU_TERMINE_JACK_A_GAGNE_!!");
								broadcastMessage(reply);
							}
						}
						else if (compteursTotJack == 16)
						{
							sprintf(reply,"T JEU_TERMINE_LES_POLICIERS_ONT_GAGNE_!!_Jack_a_mis_trop_de_temps_à_atteindre_la_zone");
							broadcastMessage(reply);
						}
					}
					tour++;
					if (tour == 4) tour = 0;
					printf("passage au tour suivant\n");
					sprintf(reply,"O %d", tour);
					broadcastMessage(reply);
					sprintf(reply,"T Au_joueur_%s_de_jouer_!!", tcpClients[tour].name);
					broadcastMessage(reply);
				}
			}
			break;
		case 'A':     // vérifie si Jack est passé par là, si il est là, ou si c'est une zone de decouverte
			sscanf(buffer,"%c %d %d", &com, &id, &position);
			printf("COM=%c id=%d position potentielle indice=%d\n", com, id, position);
			if (fsmServer == 2)
			{
				if (id == tour)
				{
					if (id != 0)  // seulement les policiers ont besoin de savoir
					{
						if (posJack == position)
						{
							sprintf(reply,"T JEU_TERMINE_LES_POLICIERS_ONT_GAGNE_!!");
							broadcastMessage(reply);	
						}
						else if (zoneDeDecouverte[position] == 1)
						{
							zoneDeDecouverte[position] = 0;
							indiceOuZone = 1;
							nbZonesDecouvertesPolice++;
							sprintf(buffer, "R %d %d", position, indiceOuZone);  // envoie la confirmation de la postion de la zone de decouverte
							broadcastMessage(buffer);
							if (nbZonesDecouvertesPolice == 4)   // toutes les zones de decouvertes ont ete trouvees
							{
								sprintf(reply,"T JEU_TERMINE_LES_POLICIERS_ONT_GAGNE_!!");
								broadcastMessage(reply);
							}
						}
						else if (jackEstPasse[position] == 1)    // priorité à la zone de découverte
						{
							indiceOuZone = 0;
							sprintf(buffer, "R %d %d", position, indiceOuZone);  // envoie la confirmation de la postion de l'indice
							broadcastMessage(buffer);
						}
						tour++;
						if (tour == 4) tour = 0;
						sprintf(reply,"O %d", tour);
						broadcastMessage(reply);
						sprintf(reply,"T Au_joueur_%s_de_jouer_!!", tcpClients[tour].name);
						broadcastMessage(reply);	
					}
				}
			}
			break;
		case 'I':     // stade initialisation : Jack choisit quatre zones de decouvertes et se place, et les trois policiers se placent
			sscanf(buffer,"%c %d %d", &com, &id, &position);
			printf("COM=%c id=%d position=%d\n", com, id, position);
			if (fsmServer == 1)
			{
				if (id == 0)   // Init Jack
				{
					if (flag1 <= 3)
					{
						zoneDeDecouverte[position] = 1;
						broadcastMessage("T Zone_de_decouverte_ajoutee");
						flag1++;
					}
					else if (flag1 == 4) 
					{
						jackEstPasse[position] = 1;
						printf("position Jack ajoutée\n");
						broadcastMessage("T Initialisation_Jack_terminee_!");
						flag1++;
					}
				}
				else if ((id == 1) && (flag2 == 0)) // Init policier 1
				{
					flag2++;
					broadcastMessage("T Initialisation_policier1_!");
					sprintf(buffer, "P %d %d", position, id);
					broadcastMessage(buffer);	
					printf("envoi de la position aux autres policiers\n");
				}
				else if ((id == 2) && (flag3 == 0))  // Init policier 2
				{
					flag3++;
					broadcastMessage("T Initialisation_policier2_terminee_!");
					sprintf(buffer, "P %d %d", position, id);
					broadcastMessage(buffer);	
					printf("envoi de la position aux autres policiers\n");
				}
				else if ((id == 3) && (flag4 == 0))  // Init policier 3
				{
					flag4++;
					broadcastMessage("T Initialisation_policier3_terminee_!");
					sprintf(buffer, "P %d %d", position, id);
					broadcastMessage(buffer);	
					printf("envoi de la position aux autres policiers\n");
				}
				if ((flag1 == 5) && (flag2 == 1) && (flag3 == 1) && (flag4 == 1))
				{
					fsmServer = 2;
					broadcastMessage("T Initialisation_terminee_!");
					printf("Phase d'initialisation terminée\n");
					sprintf(reply,"J %d", fsmServer);
					broadcastMessage(reply);

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
		
    	close(newsockfd);
    }
    close(sockfd);
    return 0; 
}
