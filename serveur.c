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
	char bienv1[NMAX]="Bienvenue Client 1\nAttendez la connection de Client 2\n";
	char bienv2[NMAX]="Bienvenue Client 2\nAttendez le message de Client 1\n";
	char msg[NMAX];
	char fin[10]="fin\n";
	char mess1[NMAX]="Vous pouvez envoyer un message : \nPour terminer tapper fin\n";
	char messfin[NMAX]="La conversation est termine\n";
	int socketferme = 1;

	//Creation de socket 1
	int dS1 = CreationSocket();

	//Creation serveur
	CreationServeur(dS1,argv[1]);
	while(1){
		//connection de la socket Client 1
		int dSC1 = ConnectionSocket(dS1);
		printf("Client 1 connecte\nEn attente de la connection de Client 2\n\n");
		Envoi(dSC1,bienv1,sizeof(bienv1));
		
		//connection de la socket Client 2
		int dSC2 = ConnectionSocket(dS1);
		printf("Client 2 connecte\nAttente du 1er message de Client 1\n\n");
		Envoi(dSC2,bienv2,sizeof(bienv2));

		//Message démarrage conversation au Client 1
		Envoi(dSC1,mess1,sizeof(mess1));

		socketferme = 1;
		//Tan qu'un client n'envoie pas 'fin'
		while(socketferme){
			//REception message client 1
			Reception(dSC1,msg,sizeof(msg));
			//Si le client 1 souhaite terminer
			if (!strcmp(msg,fin)){
				socketferme=FinConv(dSC1,dSC2,messfin,sizeof(messfin));
			}
			else{
				//Reception et reponse Client 2
				Envoi(dSC2,msg,sizeof(msg));
				Reception(dSC2,msg,sizeof(msg));

				//Si le client 2 souhaite terminer
				if (!strcmp(msg,fin)){
					socketferme=FinConv(dSC1,dSC2,messfin,sizeof(messfin));
				}
				else{
					//Envoie de la reponse a Client 1
					Envoi(dSC1,msg,sizeof(msg));
				}
				
			}
			
		}
		printf("La conversation est termine\nEn attente de la connection de nouveaux client\n\n");
	}
	
	close(dS1);
	return 0;
}

