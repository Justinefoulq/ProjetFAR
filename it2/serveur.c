#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> //close
#include "FonctionsIntermediaires.h"
#include <pthread.h>
#define NMAX 100

//Structure prise ne parametre sur fctT1
struct SocketClient{
	int dSC1;
	int dSC2;
};

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

//Variale qui dit si la conv est terminé
int socketferme = 1;

//Fonction du thread T1
void *fctT1(void* input){
	char msg[NMAX];
	char fin[10]="fin\n";
	char * messfin="La conversation est termine\n";
	//Tan que le messaage de fin n'est pas envoyé
	while(socketferme){
		//Reception du client 1
		RecMsg(((struct SocketClient*)input)->dSC1,msg);
		//si message est fin
		if (!strcmp(msg,fin)){
			socketferme=FinConv(((struct SocketClient*)input)->dSC1,((struct SocketClient*)input)->dSC2,messfin);
			if (socketferme<0){
				perror("Probleme Fin de conv\n");
			}
			socketferme=0;
		}
		else if(socketferme){
			//Reception du client 2
			EnvMsg(((struct SocketClient*)input)->dSC2,msg);
		}
	}
	//Fermeture des socket client
	close(((struct SocketClient*)input)->dSC1);
	close(((struct SocketClient*)input)->dSC2);
	pthread_exit(NULL);
	return NULL;
}


int main(int argc, char ** argv){

	//Déclaration variables utiles
	char * bienv1="Bienvenue Client 1\n";
	char * bienv2="Bienvenue Client 2\n";
	char * messfin="La conversation est termine\n";
	char msg[NMAX];
	char fin[10]="fin\n";
	char pseudo1[15];
	char pseudo2[15];
	int res;

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
		printf("client1 : %s",pseudo1);
		printf("client2 : %s",pseudo2);
		EnvMsg(dSC1,pseudo2);
		EnvMsg(dSC2,pseudo1);

		//On créer la structure
		struct SocketClient *SC = (struct SocketClient *)malloc(sizeof(struct SocketClient));
		SC->dSC1 = dSC1;
		SC->dSC2 = dSC2;

		//Création d'un thread T1 qui recois client1 et envoie client 2
		pthread_t T1;
		res = pthread_create(&T1, NULL, fctT1, (void*)SC);
		if(res<0){
			perror("Probleme cration thread 1\n");
		}
		//Recoie client 2 et envoie client1
		//tan que pas message fin
		while(socketferme){
			//recoie a client 2
			RecMsg(dSC2,msg);
			if (!strcmp(msg,fin)){
				socketferme=FinConv(dSC1,dSC2,messfin);
				if (socketferme<0){
					perror("Probleme Fin de conv\n");
				}
			}
			//envoie a client 1
			else if(socketferme){
				EnvMsg(dSC1,msg);
			}
			
		}
		//fermeture des sockets
		close(dSC1);
		close(dSC2);
		printf("La conversation est termine\nEn attente de la connection de nouveaux client\n\n");
		socketferme=1;
	}
	
	close(dS1);
	return 0;
}





