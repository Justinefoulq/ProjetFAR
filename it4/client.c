#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> //close
#include <string.h>
#include "FonctionsIntermediaires.h"
#include <pthread.h>
#include <signal.h>
#define NMAX 100

//Variale qui dit si la conv est terminé
int socketferme = 1;

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

//Structure utile pour le thread
struct SocketServeur{
	int dSock;
	char pseudo1[15];
	char pseudo2[15];
};


//Fonction du thread T1 qui recoi les messages
void *fctT1(void* input){
	char msg[NMAX];
	char fin[10]="fin\n";
	//Tan que le message n'est pas fin
	while(socketferme){
		RecMsg(((struct SocketServeur*)input)->dSock,msg);
		//Si le message est le message de fin
		if (!strcmp(msg,fin)){
			socketferme = 0;
			EnvMsg(((struct SocketServeur*)input)->dSock,fin);
		}
		//sinon on affiche
		else{
			printf("			%s: %s",((struct SocketServeur*)input)->pseudo2,msg);
		}
	}
	pthread_exit(NULL);
	
	return NULL;
}

//Fonction du thread T2 qui envoie les messages
void *fctT2(void* input){
	char msg[NMAX];
	//Tan que le message fin n'est pas envoyé
	while(socketferme){
		//Demande a l'utilisateur le message et l'envoie
		fgets(msg,NMAX,stdin);
		EnvMsg(((struct SocketServeur*)input)->dSock,msg);
	}
	pthread_exit(NULL);
	return NULL;
}


int main(int argc, char ** argv){

	//Declaration variables utiles
	int res,i;
	char msg[NMAX];
	char pseudo1[15];
	char pseudo2[15];
	char * messfin = "La conversation est termine\n";

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
	int taille;
	Reception(dSock,(char*) &taille,sizeof(int));
	printf("Voici les %d salons disponibles: \n",taille );
	for(i=0;i<taille;i++){
		RecMsg(dSock, msg);
		printf("%s\n",msg);
	}
	printf("Choisis ton salon : \n" );
	fgets(msg,NMAX,stdin);
	EnvMsg(dSock,msg);
	RecMsg(dSock,msg);
	while(strcmp(msg,"OK")!=0){
		printf("%s\n",msg);
		fgets(msg,NMAX,stdin);
		EnvMsg(dSock,msg);
		RecMsg(dSock,msg);
	}
	//Gestion des pseudos
	printf("Entrer votre pseudo :");
	fgets(pseudo1,15,stdin);
	EnvMsg(dSock,pseudo1);
	RecMsg(dSock,pseudo2);
	pseudo1[strlen(pseudo1)-1]='\0';
	pseudo2[strlen(pseudo2)-1]='\0';
	printf("La conversation est lancée avec %s\n", pseudo2);
	//structure pour le thread
	struct SocketServeur *SS = (struct SocketServeur *)malloc(sizeof(struct SocketServeur));
	SS->dSock = dSock;
	strcpy(SS->pseudo1,pseudo1);
	strcpy(SS->pseudo2,pseudo2);
	
	//Creation du thread de reception
	pthread_t T1;
	res = pthread_create(&T1, NULL, fctT1, (void*)SS);
	if(res<0){
		perror("Probleme cration thread 1\n");
	}

	//Creation du thread d'envoi
	pthread_t T2;
	res = pthread_create(&T2, NULL, fctT2, (void*)SS);
	if(res<0){
		perror("Probleme cration thread 1\n");
	}

	

	pthread_join(T1,0);
	printf("La conversation est termine\n");
	
	// close(dSock);
	pthread_kill(T2,SIGINT);
	return 0;
}