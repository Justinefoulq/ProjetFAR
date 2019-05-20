#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> //close
#include "FonctionsIntermediaires.h"
#include <pthread.h>
#define NMAX 100
#define NBSAL 3


struct SocketClient{
	int dSC1;
	int dSC2;
};

struct salon{
	char NomSalon[NMAX];
	int NbConnecte;
	int ConvoLancer;
	int SocketC1;
	int SocketC2;
};

struct ListeSal{
	struct salon * tab[NBSAL];
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


//Fonction du thread T1
//C1 -> C2
void *fctT1(void* input){
	char msg[NMAX];
	char fin[10]="fin\n";
	//Tan que le messaage de fin n'est pas envoyé
	while(1){
		//Reception du client 1
		RecMsg(((struct SocketClient*)input)->dSC1,msg);
		EnvMsg(((struct SocketClient*)input)->dSC2,msg);
	}
	pthread_exit(NULL);
	return NULL;
}

//Fonction du thread T2
//C2 -> C1
void *fctT2(void* input){
	char msg[NMAX];
	char fin[10]="fin\n";
	
	//Tan que le messaage de fin n'est pas envoyé
	while(1){
		//Reception du client 2
		RecMsg(((struct SocketClient*)input)->dSC2,msg);
		EnvMsg(((struct SocketClient*)input)->dSC1,msg);
	}
	pthread_exit(NULL);
	return NULL;
}



int main(int argc, char ** argv){

	//Déclaration variables utiles
	char * bienv="Choisis un salon dans la liste:\n";
	char * NulSalon="Desolé ce nom n'est pas valide merci d'en choisir un autre";
	char * BonSalon="OK";
	char * messfin="La conversation est termine\n";
	char msg[NMAX];
	char fin[10]="fin\n";
	char pseudo1[15];
	char pseudo2[15];
	int res,i;

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

	struct ListeSal *LS = (struct ListeSal *)malloc(sizeof(struct ListeSal));

	for(i=0;i<NBSAL;i++){
		struct salon *Sal = (struct salon *)malloc(sizeof(struct salon));
		Sal->NbConnecte=0;
		Sal->ConvoLancer=0;
		printf("Rentrer le nom du salon n°%d\n", i+1);
		fgets(msg,NMAX,stdin);
		msg[strlen(msg)-1]='\0';
		strcpy(Sal->NomSalon,msg);
		LS->tab[i]=Sal;
	}
	printf("En attente du 1er client\n");
	int j=0;
	while(j<NBSAL*2){
		//connection de la socket Client 1
		int dSC = ConnexionSocket(dS1);
		if (dSC <0){
			perror("Probleme connexion SocketC1\n");
		}
		EnvMsg(dSC,bienv);
		printf("Client connecte\nEn attente de son choix\n\n");
		int taille=0;
		for(i=0; i<NBSAL;i++){
			if(LS->tab[i]->ConvoLancer==0){
				taille = taille+1;
			}
		}
		Envoi(dSC,(char*) &taille,sizeof(int));
		for(i=0; i<NBSAL;i++){
			if(LS->tab[i]->ConvoLancer==0){
				EnvMsg(dSC,LS->tab[i]->NomSalon);
			}
			// printf("%s\n",LS->tab[i]->NomSalon );
		}
		int bonsalon=0;
		do{
			RecMsg(dSC,msg);
			msg[strlen(msg)-1]='\0';
			for (i=0; i<NBSAL; i++){
				if(!strcmp(msg,LS->tab[i]->NomSalon)){
					bonsalon=1;
					if(LS->tab[i]->NbConnecte==0){
						LS->tab[i]->SocketC1=dSC;
						LS->tab[i]->NbConnecte++;
						EnvMsg(dSC,BonSalon);

					}
					else if(LS->tab[i]->NbConnecte==1){
						LS->tab[i]->SocketC2=dSC;
						LS->tab[i]->NbConnecte++;
						EnvMsg(dSC,BonSalon);
					}
					else{
						bonsalon=0;
					}
				}
			}
			if(!bonsalon){
				EnvMsg(dSC,NulSalon);
			}
		}while(!bonsalon);
		for (i=0;i<NBSAL;i++){
			if(LS->tab[i]->ConvoLancer==0 && LS->tab[i]->NbConnecte==2){
				//Reception + envoie pseudo1
				RecMsg(LS->tab[i]->SocketC1,pseudo1);
				RecMsg(LS->tab[i]->SocketC2,pseudo2);
				printf("client1 : %s",pseudo1);
				printf("client2 : %s",pseudo2);
				EnvMsg(LS->tab[i]->SocketC1,pseudo2);
				EnvMsg(LS->tab[i]->SocketC2,pseudo1);

				//On créer la structure
				struct SocketClient *SC = (struct SocketClient *)malloc(sizeof(struct SocketClient));
				SC->dSC1 = LS->tab[i]->SocketC1;
				SC->dSC2 = LS->tab[i]->SocketC2;

				//Création d'un thread T1 qui recoi client1 et envoie client2
				pthread_t T1;
				res = pthread_create(&T1, NULL, fctT1, (void*)SC);
				if(res<0){
					perror("Probleme cration thread 1\n");
				}

				//Création d'un thread T2 qui recoi client2 at envoie client1
				pthread_t T2;
				res = pthread_create(&T2, NULL, fctT2, (void*)SC);
				if(res<0){
					perror("Probleme cration thread 1\n");
				}
				LS->tab[i]->ConvoLancer=1;
			}
		}
	}
	
	close(dS1);
	return 0;
}





