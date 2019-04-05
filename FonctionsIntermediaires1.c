#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> //close
#include <string.h>
#define NMAX 100


int CreationSocket(){
	int dSock = socket(PF_INET, SOCK_STREAM, 0);
	if (dSock < 0 ){
		perror("Probleme creation socket");
		exit(1);
	}
	return dSock;
}

void CreationServeur(int dS, char *port){
	struct sockaddr_in ad ;//definir le type d'addresse
	ad.sin_family = AF_INET ;//IPV4
	ad.sin_addr.s_addr = INADDR_ANY ;//pour associer nimpoprte quel IP
	ad.sin_port = htons((short)atoi(port)) ;//Associer num port
	int res = bind(dS, (struct sockaddr*)&ad, sizeof(ad)) ; //Renomage
	if (res<0){
		perror("Probleme de renommage socket 1");
		exit(1);
	}
	printf("Serveur operationnel\n\n");
}

void ConnexionServeur(int dS, char *port, char *IP){
	struct sockaddr_in adServ ;
	adServ.sin_family = AF_INET ; 
	adServ.sin_port = htons((short)atoi(port)) ;
	int res = inet_pton(AF_INET, IP, &(adServ.sin_addr)) ; 
	socklen_t lgA = sizeof(struct sockaddr_in) ;
	res = connect(dS, (struct sockaddr *) &adServ, lgA) ; 
	if (res<0){
		perror("Probleme de connection au serveur");
		exit(1);
	}
}

int ConnexionSocket(int dS){
	int res = listen(dS, 5) ;//nb max de demande de connection
	if (res<0){
		perror("Probleme de listen socket 1");
		exit(1);
	}
	struct sockaddr_in aC1 ; //adresse de la socket du client
	socklen_t lg1 = sizeof(struct sockaddr_in) ;//longueur de l'adresse
	int dSC1= accept(dS, (struct sockaddr*) &aC1,&lg1);//dS1 demande connection
	if (dSC1 < 0 ){
		perror("Probleme connection socket C1");
		exit(1);
	}
	return dSC1;
}

int Envoi(int dSC, char* msg, int taille){
	int res;
	int somme=0;
	while(somme<taille){
		res=send(dSC,msg+somme,taille-somme,0);
		if (res<0){
			return 1;
		}
		else if(res==0){
			return 2;
		}
		somme=somme+res;
	}
	return 0;
}
/*
int EnvoiMsg(int dSC,char* msg){
	int taille = strlen(msg)+1;
	int res1=Envoi(dSC,(char*) &taille,sizeof(int));
	int res2=Envoi(dSC,msg,taille);
	if(res1==0 && res2==0){
		return 0;
	}
	else if(res1==1 || res2==1){
		return 1;
	}
	else return 2;
}*/
 
 /*

int Reception(int dSC,char* msg){
	int res;
	int somme=0;
	int taille;
	res = recv(dSC,(char*) &taille,sizeof(int),0);
	if (res<0){
		return 1;
	}
	else if(res==0){
		return 2;
	}
	while(somme<taille){
		res=recv(dSC,msg+somme,taille-somme,0);
		if (res<0){
			return 1;
		}
		else if(res==0){
			return 2;
		}
		somme=somme+res;
	}
	return 0;
}
*/

int Reception(int dSC, char* msg,int taille){
	int res,somme=0;
	while(somme<taille){
		res=recv(dSC,msg+somme,taille-somme,0);
		if (res<0){
			return 1;
		}
		else if(res==0){
			return 2;
		}
		somme=somme+res;
	}
	return 0;
}

int FinConv(int dSC1,int dSC2,char * messfin){
	int taille,res;
	taille = strlen(messfin)+1;
	res=Envoi(dSC1,(char*) &taille,sizeof(int));
	res=Envoi(dSC1,messfin,taille);
	taille = strlen(messfin)+1;
	res=Envoi(dSC2,(char*) &taille,sizeof(int));
	res=Envoi(dSC2,messfin,taille);
	//On coupe la connection des 2 clients
	close(dSC1);
	close(dSC2);
	return 0;
}

