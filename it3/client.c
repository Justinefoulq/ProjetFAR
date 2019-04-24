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
#include <limits.h>
#include <dirent.h>
#define NMAX 100
#define FMAX 10000

//Variale qui dit si la conv est terminé
int socketferme = 1;

int get_last_tty() {
  FILE *fp;
  char path[1035];
  fp = popen("/bin/ls /dev/pts", "r");
  if (fp == NULL) {
    printf("Impossible d'exécuter la commande\n" );
    exit(1);
  }
  int i = INT_MIN;
  while (fgets(path, sizeof(path)-1, fp) != NULL) {
    if(strcmp(path,"ptmx")!=0){
      int tty = atoi(path);
      if(tty > i) i = tty;
    }
  }

  pclose(fp);
  return i;
}

FILE* new_tty() {
  pthread_mutex_t the_mutex;  
  pthread_mutex_init(&the_mutex,0);
  pthread_mutex_lock(&the_mutex);
  system("gnome-terminal"); sleep(1);
  char *tty_name = ttyname(STDIN_FILENO);
  int ltty = get_last_tty();
  char str[2];
  sprintf(str,"%d",ltty);
  int i;
  for(i = strlen(tty_name)-1; i >= 0; i--) {
    if(tty_name[i] == '/') break;
  }
  tty_name[i+1] = '\0';  
  strcat(tty_name,str);  
  FILE *fp = fopen(tty_name,"wb+");
  pthread_mutex_unlock(&the_mutex);
  pthread_mutex_destroy(&the_mutex);
  return fp;
}

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

//Fonction du thread T3 qui recoit le nom du fichier puis son contenu
void *fctT3(void* input){
	char msg[FMAX];
	char nom[NMAX];
	char dos[20]="Reception/";
	int dS=((struct SocketServeur*)input)->dSock;
	RecMsg(dS,nom);
	printf("On vous envoi le fichier %s\n",nom);
	FILE *f = NULL;
	nom[strlen(nom)-1]='\0';
	strcat(dos,nom);
	f=fopen(dos,"w");
	RecMsg(dS,msg);
	fputs(msg,f);
	fclose(f);
	printf("Vous avez recu le fichier %s\n",nom);
	pthread_exit(NULL);
	return NULL;
}

//Fonction du thread T4 qui demande le nom du fichier contenu dans le dossier, envoi le nom du fichier puis son contenu
void *fctT4(void* input){
	char msg[FMAX]="";
	char nom[NMAX];
	int dS=((struct SocketServeur*)input)->dSock;
	FILE* fp1 = new_tty();
  	fprintf(fp1,"%s\n","Ce terminal sera utilisé uniquement pour l'affichage");
	DIR *dp;
	struct dirent *ep;     
	dp = opendir ("./Envoi/");
	if (dp != NULL) {
		fprintf(fp1,"Voilà la liste de fichiers :\n");
		while (ep = readdir (dp)) {
			if(strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0) 
				fprintf(fp1,"%s\n",ep->d_name);
			}    
			(void) closedir (dp);
		}
	else {
		perror ("Ne peux pas ouvrir le répertoire");
	}
	printf("Indiquer le nom du fichier : ");
	char fileName[1023];
	fgets(fileName,sizeof(fileName),stdin);
	EnvMsg(dS,fileName);
	fileName[strlen(fileName)-1]='\0';
	FILE *fps = fopen(fileName, "r");
	if (fps == NULL){
		printf("Ne peux pas ouvrir le fichier suivant : %s",fileName);
	}
	else {
		char str[1000];    
		// Lire et afficher le contenu du fichier
		while (fgets(str, 1000, fps) != NULL) {
			fprintf(fp1,"%s",str);
			strcat(msg,str);
		}
		printf("Je suis sorti du while\n");
		EnvMsg(dS,msg);
		printf("J'ai envoyer texte fichier\n");
	}
	fclose(fps);	
	pthread_exit(NULL);
	return NULL;
}


//Fonction du thread T1 qui recoit les messages
void *fctT1(void* input){
	char msg[NMAX];
	char * messfin = "La conversation est termine\n";
	char * messfile = "file\n";
	int res;
	struct SocketServeur *SS =((struct SocketServeur*)input);
	//Tan que le message n'est pas fin
	while(socketferme){
		RecMsg(SS->dSock,msg);
		//Si le message est le message de fin
		if (!strcmp(msg,messfin)){
			socketferme = 0;
		}
		else if(!strcmp(msg,messfile)){
			printf("J'ai recu file\n");
			pthread_t T3;
			res = pthread_create(&T3, NULL, fctT3, (void*)SS);
			if(res<0){
				perror("Probleme cration thread 1\n");
			}
			pthread_join(T3,0);
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
	char fin[10]="fin\n";
	char * messfile = "file\n";
	int res;
	struct SocketServeur *SS =((struct SocketServeur*)input);
	//Tan que le message fin n'est pas envoyé
	while(socketferme){
		//Demande a l'utilisateur le message et l'envoie
		fgets(msg,NMAX,stdin);
		EnvMsg(SS->dSock,msg);
		if(!strcmp(msg,messfile)){
			pthread_t T4;
			res = pthread_create(&T4, NULL, fctT4, (void*)SS);
			if(res<0){
				perror("Probleme cration thread 1\n");
			}
			pthread_join(T4,0);
		}
	}
	pthread_exit(NULL);
	return NULL;
}


int main(int argc, char ** argv){

	//Declaration variables utiles
	int res;
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

	//Gestion des pseudos
	printf("Entrer votre pseudo :");
	fgets(pseudo1,15,stdin);
	EnvMsg(dSock,pseudo1);
	RecMsg(dSock,pseudo2);
	pseudo1[strlen(pseudo1)-1]='\0';
	pseudo2[strlen(pseudo2)-1]='\0';

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
	close(dSock);
	pthread_kill(T2,SIGINT);
	return 0;
}
