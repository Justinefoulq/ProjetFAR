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
	char pseudo[15];
	int num;
};

//Fonction du thread T1 qui recoi les messages
void *fctT1(void* input){
	char msg[NMAX];
	char * messfin = "La conversation est termine\n";
	//Tan que le message n'est pas fin
	while(socketferme){
		RecMsg(((struct SocketServeur*)input)->dSock,msg);
		//Si le message est le message de fin
		if (!strcmp(msg,messfin)){
			socketferme = 0;
		}
		//sinon on affiche
		else{
			printf("			 %s",msg);
		}
	}
	pthread_exit(NULL);
	
	return NULL;
}

//Fonction du thread T2 qui envoie les messages
void *fctT2(void* input){
	char msg[NMAX];
	char tmp[NMAX];
	char fin[10]="fin\n";
	//Tan que le message fin n'est pas envoyé
	while(socketferme){
		//Demande a l'utilisateur le message et l'envoie
		fgets(msg,NMAX,stdin);
		if(!strcmp(msg,fin)){
			EnvMsg(((struct SocketServeur*)input)->dSock,msg);
		}
		else{
			sprintf(tmp,"%s : %s",((struct SocketServeur*)input)->pseudo,msg);
			EnvMsg(((struct SocketServeur*)input)->dSock,tmp);
		}
	}
	pthread_exit(NULL);
	return NULL;
}


int main(int argc, char ** argv){

	//Declaration variables utiles
	int res;
	char msg[NMAX];
	char * messfin = "La conversation est termine\n";
	
	//Creation Socket
	int dSock = CreationSocket();
	if (dSock == -1){
		perror("Probleme de création socket client\n");
	}

	//structure pour le thread
	struct SocketServeur *SS = (struct SocketServeur *)malloc(sizeof(struct SocketServeur));
	SS->dSock = dSock;
	
	//Connection au serveur
	res = ConnexionServeur(dSock,argv[1],argv[2]);
	if (res<0){
		perror("Probleme Connexion au serveur\n");
	}
	//reception mess bienv
	RecMsg(dSock, msg);
	recv(dSock,(char*) &SS->num,sizeof(int),0);
	printf("%s %d\n",msg,SS->num);
	printf("Entrer votre pseudo :");
	fgets(SS->pseudo,15,stdin);
	SS->pseudo[strlen(SS->pseudo)-1]='\0';
	
	
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
	close(dSock);
	pthread_kill(T2,SIGINT);
	return 0;
}