#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> //close
#include "FonctionsIntermediaires.h"
#define NMAX 100

int main(int argc, char ** argv){

	//Déclaration variables utiles
	char * bienv1="Bienvenue Client 1\nAttendez la connection de Client 2\n";
	char * bienv2="Bienvenue Client 2\nAttendez le message de Client 1\n";
	char * msg;
	char fin[10]="fin\n";
	char * mess1="Vous pouvez envoyer un message : \nPour terminer tapper fin\n";
	char * messfin="La conversation est termine\n";
	int socketferme = 1;
	int res,taille;

	//Creation de socket 1
	int dS1 = CreationSocket();

	//Creation serveur
	CreationServeur(dS1,argv[1]);
	while(1){
		//connection de la socket Client 1
		int dSC1 = ConnexionSocket(dS1);
		taille = strlen(bienv1)+1;
		res=Envoi(dSC1,(char*) &taille,sizeof(int));
		res=Envoi(dSC1,bienv1,taille);
		printf("Client 1 connecte\nEn attente de la connection de Client 2\n\n");
		
		//connection de la socket Client 2
		int dSC2 = ConnexionSocket(dS1);
		taille = strlen(bienv2)+1;
		res=Envoi(dSC2,(char*) &taille,sizeof(int));
		res=Envoi(dSC2,bienv2,taille);
		printf("Client 2 connecte\nAttente du 1er message de Client 1\n\n");

		//Message démarrage conversation au Client 1
		taille = strlen(mess1)+1;
		res=Envoi(dSC1,(char*) &taille,sizeof(int));
		res=Envoi(dSC1,mess1,taille);

		socketferme = 1;
		//Tan qu'un client n'envoie pas 'fin'
		while(socketferme){
			//REception message client 1
			res = recv(dSC1,(char*) &taille,sizeof(int),0);
			res = Reception(dSC1,msg,taille);
			//Si le client 1 souhaite terminer
			if (!strcmp(msg,fin)){
				socketferme=FinConv(dSC1,dSC2,messfin);
			}
			else{
				//Reception et reponse Client 2
				taille = strlen(msg)+1;
				res=Envoi(dSC2,(char*) &taille,sizeof(int));
				res=Envoi(dSC2,msg,taille);
				res = recv(dSC2,(char*) &taille,sizeof(int),0);
				res = Reception(dSC2,msg,taille);

				//Si le client 2 souhaite terminer
				if (!strcmp(msg,fin)){
					socketferme=FinConv(dSC1,dSC2,messfin);
				}
				else{
					//Envoie de la reponse a Client 1
					taille = strlen(msg)+1;
					res=Envoi(dSC1,(char*) &taille,sizeof(int));
					res=Envoi(dSC1,msg,taille);
				}
			}
		}
		printf("La conversation est termine\nEn attente de la connection de nouveaux client\n\n");
	}
	
	close(dS1);
	return 0;
}

