#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> //close
#include "FonctionsIntermediaires.h"
#include <pthread.h>
#include <signal.h>
#define NMAX 100
#define CMAX 100

//Structure prise ne parametre sur fctT1
struct SocketClient{
	int dSC[CMAX];
	int nbC;
};

//Fonction d'envoi d'un message
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

//Fonction de reception d'un message
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
//Socket serveur
int dS;

//Fonction du thread T1 qui correspond au thread d'un client qui recoit un message et l'envoie a tout les autres client
void *fctT1(void* input){
	char msg[NMAX];
	char fin[10]="fin\n";
	int i;
	struct SocketClient *SocketC =((struct SocketClient*)input);
	//Tan que le messaage de fin n'est pas envoyé
	int numClient=SocketC->nbC-1;
	while(socketferme){
		//Reception du client 1
		RecMsg(SocketC->dSC[numClient],msg);
		//si message est fin on mets din a la conversation en envoyant a tout le groupe le message de fin
		if (!strcmp(msg,fin)){
			socketferme=FinGroupe(SocketC->dSC,SocketC->nbC);
			if (socketferme<0){
				perror("Probleme Fin de groupe\n");
			}
			socketferme=0;
		}
		//Sinon on envoi a tout les client sauf celui qui a envoyé le message recu
		else if(socketferme){
			for(i=0;i<SocketC->nbC;i++){
				if(i!=numClient){
					EnvMsg(SocketC->dSC[i],msg);	
				}
			}
		}
	}
	pthread_exit(NULL);
	return NULL;
}

//Fonction du thread T2 qui créer jusqu'à 100 client avec leurs thread T1 correspondant
void *fctT2(void* input){
	char * bienv="Bienvenue";
	int res;
	struct SocketClient *SocketC =((struct SocketClient*)input);
	//Tant qu'on a moins de 100 clients
	while((SocketC->nbC)<100){
		//Connexion du client n
		SocketC->dSC[SocketC->nbC]= ConnexionSocket(dS);
		if(SocketC->dSC[SocketC->nbC] == -1){
			perror("Probleme connexion SocketC\n");
		}
		//Envoie message de bienvenu avec le num du client 
		EnvMsg(SocketC->dSC[SocketC->nbC],bienv);
		printf("Client %d connecte\n",SocketC->nbC);
		Envoi(SocketC->dSC[SocketC->nbC],(char*) &SocketC->nbC,sizeof(int));
		((struct SocketClient*)input)->nbC++;
		//Creation du thread du client n qui gere la reception et l'envoie des mess
		pthread_t T1;
		res = pthread_create(&T1, NULL, fctT1, (void*)SocketC);
		if(res<0){
			perror("Probleme cration thread 1\n");
		}
	}
	pthread_exit(NULL);
	return NULL;
}




int main(int argc, char ** argv){

	//Déclaration variables utiles
	char * bienv="Bienvenue";
	char * messfin="La conversation est termine\n";
	char msg[NMAX];
	char fin[10]="fin\n";
	int res;
	struct SocketClient *SocketC = (struct SocketClient *)malloc(sizeof(struct SocketClient));
	SocketC->nbC=0;

	//Creation de socket 
	dS = CreationSocket();
	if (dS == -1){
		perror("Probleme de création socket serveur\n");
	}
	//Creation serveur
	res = CreationServeur(dS,argv[1]);
	if (res == -1){
		perror("Probleme de création serveur\n");
	}


	while(1){
		//Creation du thread T2 qui connecte les clients et créer leurs thread
		pthread_t T2;
		res = pthread_create(&T2,NULL, fctT2, (void*)SocketC);
		if(res<0){
				perror("Probleme cration thread 2\n");
		}
		

		//On attend La fermeture des 2 thread
		while(socketferme){
		}
		
		pthread_cancel(T2);
		SocketC->nbC=0;

		//Fermueture des socket
	
		printf("\nLa conversation est termine\nEn attente de la connection de nouveaux client\n\n");
		socketferme=1;
		
	}
	
	close(dS);
	return 0;
}





