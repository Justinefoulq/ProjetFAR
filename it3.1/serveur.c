#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> //close
#include "FonctionsIntermediaires.h"
#include <pthread.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#define NMAX 100


int main(int argc, char ** argv){
	int res;
	socklen_t lg = sizeof(struct sockaddr_in) ;//longueur de l'adresse
	char ipC1[50];
	char ipC2[50];
	struct sockaddr_in aC1 ; //adresse de la socket du client
	struct sockaddr_in aC2 ; //adresse de la socket du client
	char ipclient[50];
	char pseudo1[15];
	char pseudo2[15];
	char rep[10];
	int conv=1;
	while(conv){
		//Creation de socket 1
		conv=0;
		int dS = socket(PF_INET, SOCK_DGRAM, 0);
		if (dS == -1){
			perror("Probleme de création socket serveur\n");
		}

		//Creation serveur
		res = CreationServeur(dS,argv[1]);
		if (res == -1){
			perror("Probleme de création serveur\n");
		}

		printf("En attente d'un message de client 1\n");
		recvfrom(dS,pseudo1,sizeof(pseudo1),0,(struct sockaddr*) &aC1, &lg);
		inet_ntop(AF_INET, &(aC1.sin_addr), ipclient, INET_ADDRSTRLEN);
		printf("Client 1 : %sIP: %s\nPort: %d\n\n",pseudo1,inet_ntoa(aC1.sin_addr),(int)ntohs(aC1.sin_port));
		
		printf("En attente d'un message de client 2\n");
		recvfrom(dS,pseudo2,sizeof(pseudo2),0,(struct sockaddr*) &aC2, &lg);
		inet_ntop(AF_INET, &(aC2.sin_addr), ipclient, INET_ADDRSTRLEN);

		printf("Client 2 : %sIP: %s\nPort: %d\n\n",pseudo2,inet_ntoa(aC2.sin_addr),(int)ntohs(aC2.sin_port));
		//Envoyer aC1 à C2 et envoyer aC2 à C1
		sendto(dS,(struct sockaddr*) &aC2,lg,0,(struct sockaddr*) &aC1, lg);
		sendto(dS,(struct sockaddr*) &aC1,lg,0,(struct sockaddr*) &aC2, lg);
		//Envoie des pseudo
		sendto(dS,pseudo2,strlen(pseudo2)+1,0,(struct sockaddr*) &aC1, lg);
		sendto(dS,pseudo1,strlen(pseudo1)+1,0,(struct sockaddr*) &aC2, lg);

		//Demande de nouvelle conv
		printf("Voulez vous lancer une nouvelle conversation ? (O/N)\n");
		fgets(rep,10,stdin);
		if(strcmp(rep,"o\n")==0 || strcmp(rep,"0\n")==0){
			conv=1;
			printf("La nouvelle conversation va etre lance\n\n");
		}
		close(dS);
	}
	printf("\nFin de creation des conversation\n");
	

}