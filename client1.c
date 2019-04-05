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
	char * msg;
	char * mot;
	char * messfin = "La conversation est termine\n";
	int socketferme = 1;
	int taille;

	//Creation Socket
	int dSock = CreationSocket();
	
	//Connection au serveur
	ConnexionServeur(dSock,argv[1],argv[2]);
	
	res = recv(dSock,(char*) &taille,sizeof(int),0);
	res = Reception(dSock,msg,taille);
	printf("%s\n",msg);
	
	//Tant qu'un client n'envoie pas 'fin'
	while(socketferme){
		//Ecrit le message recu
		res = recv(dSock,(char*) &taille,sizeof(int),0);
		res = Reception(dSock,msg,taille);
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
			taille = strlen(mot)+1;
			res=Envoi(dSock,(char*) &taille,sizeof(int));
			res=Envoi(dSock,mot,taille);
		}
		
	}

	return 0;
}