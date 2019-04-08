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

void ConnectionServeur(int dS, char *port, char *IP){
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

int ConnectionSocket(int dS){
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

void Envoi(int dSC,char msg[NMAX], int taille){
	int res= send(dSC,msg,taille,0);
	if (res<0){
		perror("Probleme d'envoie message bienv1");
		exit(1);
	}
}

void Reception(int dSC,char msg[NMAX], int taille){
	int res = recv(dSC, msg, taille, 0) ;
	if (res<0){
		perror("Probleme de reception message client 1");
		exit(1);
	}
}

int FinConv(int dSC1,int dSC2,char messfin[NMAX], int taille){
	Envoi(dSC1,messfin,taille);
	Envoi(dSC2,messfin,taille);
	//On coupe la connection des 2 clients
	close(dSC1);
	close(dSC2);
	return 0;
}

1
