#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> //close
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

	//Déclaration variables utiles
	char * bienv1="Bienvenue Client 1\nAttendez la connection de Client 2\n";
	char * bienv2="Bienvenue Client 2\nAttendez le message de Client 1\n";
	char * mess1="C'est parti\n";
	char * messfin="La conversation est termine\n";
	char msg[NMAX];
	char fin[10]="fin\n";
	char pseudo1[15];
	char pseudo2[15];
	int socketferme = 1;
	int res,taille;

	//Creation de socket 1
	int dS1 = CreationSocket();
	if (dS1 == -1){
		perror("Probleme de création socket serveur\n");
	}

	//Creation serveur
	res = CreationServeur(dS1,argv[1]);
	if (res == -1){
		perror("Probleme de création serveur\n");
	}

	while(1){

		//connection de la socket Client 1
		int dSC1 = ConnexionSocket(dS1);
		if (dSC1 <0){
			perror("Probleme connexion SocketC1\n");
		}
		EnvMsg(dSC1,bienv1);
		printf("Client 1 connecte\nEn attente de la connexion de Client 2\n\n");
		
		//connection de la socket Client 2
		int dSC2 = ConnexionSocket(dS1);
		if (dSC2<0){
			perror("Probleme connexion SocketC2\n");
		}
		EnvMsg(dSC2,bienv2);
		printf("Client 2 connecte\n\n");

		//Reception + envoie pseudo1
		RecMsg(dSC1,pseudo1);
		RecMsg(dSC2,pseudo2);
		EnvMsg(dSC1,pseudo2);
		EnvMsg(dSC2,pseudo1);

		//Message démarrage conversation au Client 1
		EnvMsg(dSC1,mess1);

		socketferme = 1;
		//Tan qu'un client n'envoie pas 'fin'
		while(socketferme){
			//REception message client 1
			RecMsg(dSC1,msg);

			//Si le client 1 souhaite terminer
			if (!strcmp(msg,fin)){
				socketferme=FinConv(dSC1,dSC2,messfin);
				if (socketferme<0){
					perror("Probleme Fin de conv\n");
				}
			}

			else{
				//Reception et reponse Client 2
				EnvMsg(dSC2,msg);
				RecMsg(dSC2,msg);

				//Si le client 2 souhaite terminer
				if (!strcmp(msg,fin)){
					socketferme=FinConv(dSC1,dSC2,messfin);
					if (socketferme<0){
						perror("Probleme Fin de conv\n");
					}
				}
				else{
					//Envoie de la reponse a Client 1
					EnvMsg(dSC1,msg);
				}
			}
		}
		printf("La conversation est termine\nEn attente de la connection de nouveaux client\n\n");
	}
	
	close(dS1);
	return 0;
}





