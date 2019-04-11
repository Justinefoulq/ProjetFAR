#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> //close
#include <string.h>
#include "FonctionsIntermediaires.h"
#define NMAX 100


void EnvMsg(int dS, char* msg){
	int taille, res;
	taille = strlen(msg)+1;
	res=Envoi(dS,(char*) &taille,sizeof(int));
	if (res < 0){
		perror("Probleme d'envoie de la taille du message\n");
	}
	res=Envoi(dS,msg,taille);
	if (res < 0){
		perror("Probleme d'envoie du message");
	}
}

void RecMsg(int dS, char* msg){
	int taille, res;
	res = recv(dS,(char*) &taille,sizeof(int),0);
	if (res<0){
		perror("Probleme reception taille du message\n");
	}
	res = Reception(dS,msg,taille);
	if (res<0){
		perror("Probleme reception message bienv\n");
	}
}

int main(int argc, char ** argv){

	//Declaration variables utiles
	int res;
	char msg[NMAX];
	char pseudo1[15];
	char pseudo2[15];
	char * messfin = "La conversation est termine\n";
	int socketferme = 1;

	//Creation Socket
	int dSock = CreationSocket();
	if (dSock == -1){
		perror("Probleme de création socket client\n");
	}
	
	//Connection au serveur
	res = ConnexionServeur(dSock,argv[1],argv[2]);
	if (res<0){
		perror("Probleme Connexion au serveur\n");
	}
	RecMsg(dSock, msg);
	printf("%s\n",msg);

	//Gestion des pseudos
	printf("				Entrer votre pseudo :");
	fgets(pseudo1,15,stdin);
	EnvMsg(dSock,pseudo1);
	RecMsg(dSock,pseudo2);
	pseudo1[strlen(pseudo1)-1]='\0';
	pseudo2[strlen(pseudo2)-1]='\0';
	
	
	//Tant qu'un client n'envoie pas 'fin'
	while(socketferme){

		//Ecrit le message recu
		RecMsg(dSock,msg);
		printf("%s : %s",pseudo2,msg);

		//Si le message est fin on ferme la socket
		if (!strcmp(msg,messfin)){
			close(dSock);
			socketferme = 0;
		}
		else{
			//Envoie le message tapper par le client
			printf("					%s : ",pseudo1);
			//printf("					:");
			fgets(msg,NMAX,stdin);
			EnvMsg(dSock,msg);
		}
		
	}

	return 0;
}