#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> //close
#include <string.h>
#include "FonctionsIntermediaires.h"
#define NMAX 100


int main(int argc, char ** argv){

	//Declaration variables utiles
	int res;
	char msg[NMAX];
	char mot[NMAX];
	char messfin[NMAX]="La conversation est termine\n";
	int socketferme = 1;

	//Creation Socket
	int dSock = CreationSocket();
	
	//Connection au serveur
	ConnectionServeur(dSock,argv[1],argv[2]);
	
	Reception(dSock,msg,sizeof(msg));
	printf("%s\n",msg);
	
	//Tant qu'un client n'envoie pas 'fin'
	while(socketferme){
		//Ecrit le message recu
		Reception(dSock,msg,sizeof(msg));
		printf("%s",msg);

		//Si le message est fin on ferme la socket
		if (!strcmp(msg,messfin)){
			close(dSock);
			socketferme = 0;
		}
		else{
			//Envoie le message tapper par le client
			printf("					");
			fgets(mot,NMAX,stdin);
			Envoi(dSock,mot,sizeof(mot));
		}
		
	}

	return 0;
}