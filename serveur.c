#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> //close
#define NMAX 100

int main(int argc, char ** argv){

	//Creation de socket 1
	int dS1= socket(PF_INET, SOCK_STREAM, 0) ; 
	if (dS1 < 0 ){
		perror("Probleme creation socket 1");
		return -1;
	}

	/*
	//Creation socket 2
	int dS2= socket(PF_INET, SOCK_STREAM, 0) ; 
	if (dS2 < 0 ){
		perror("Probleme creation socket 2");
		return -1;
	}
	*/

	//Creation serveur
	struct sockaddr_in ad ;//definir le type d'addresse
	ad.sin_family = AF_INET ;//IPV4
	ad.sin_addr.s_addr = INADDR_ANY ;//pour associer nimpoprte quel IP
	ad.sin_port = htons((short)atoi(argv[1])) ;//Associer num port
	int res = bind(dS1, (struct sockaddr*)&ad, sizeof(ad)) ; //Renomage
	if (res<0){
		perror("Probleme de renommage socket 1");
		return -1;
	}
	printf("Serveur operationnel\n\n");
	

	//connection de la socket Client 1
	res = listen(dS1, 5) ;//nb max de demande de connection
	if (res<0){
		perror("Probleme de listen socket 1");
		return -1;
	}
	struct sockaddr_in aC1 ; //adresse de la socket du client
	socklen_t lg1 = sizeof(struct sockaddr_in) ;//longueur de l'adresse
	int dSC1= accept(dS1, (struct sockaddr*) &aC1,&lg1);//dS1 demande connection
	if (dSC1 < 0 ){
		perror("Probleme connection socket C1");
		return -1;
	}
	printf("Client 1 connecte\n");
	printf("En attente de la connection de Client 2\n\n");
	char bienv1[NMAX]="Bienvenu Client 1\nAttendez la connection de Client 2\n";
	res= send(dSC1,bienv1,sizeof(bienv1),0);
	if (res<0){
		perror("Probleme d'envoie message bienv1");
		return -1;
	}
	
	//connection de la socket Client 2
	res = listen(dS1, 5) ;//nb max de demande de connection
	if (res<0){
		perror("Probleme de listen socket 2");
		return -1;
	}
	struct sockaddr_in aC2 ; //adresse de la socket du client
	socklen_t lg2 = sizeof(struct sockaddr_in) ;//longueur de l'adresse
	int dSC2= accept(dS1, (struct sockaddr*) &aC2,&lg2);//dS1 demande connection
	if (dSC2 < 0 ){
		perror("Probleme connection socket C2");
		return -1;
	}
	printf("Client 2 connecte\nAttente du 1er message de Client 1\n\n");
	char bienv2[NMAX]="Bienvenu Client 2\nAttendez que Client 1 envoie un message\n";
	res= send(dSC2,bienv2,sizeof(bienv2),0);
	if (res<0){
		perror("Probleme d'envoie message bienv2");
		return -1;
	}

	char msg[NMAX] ;
	char mess1[NMAX]="Vous pouvez envoyer un message : \n";
	res= send(dSC1,mess1,sizeof(mess1),0);
	if (res<0){
		perror("Probleme d'envoie message mess1");
		return -1;
	}
	while(1){
		res = recv(dSC1, msg, sizeof(msg), 0) ;
		if (res<0){
			perror("Probleme de reception message 1");
			return -1;
		}
		if (msg=="fin"){
			close(dSC1);
			close(dSC2);
		}
		res= send(dSC2,msg,sizeof(msg),0);
		if (res<0){
			perror("Probleme d'envoie message mess1");
			return -1;
		}
		
		res = recv(dSC2, msg, sizeof(msg), 0) ;
		if (res<0){
			perror("Probleme de reception message 1");
			return -1;
		}
		if (msg=="fin"){
			close(dSC1);
			close(dSC2);
		}
		res= send(dSC1,msg,sizeof(msg),0);
		if (res<0){
			perror("Probleme d'envoie message mess1");
			return -1;
		}
	}
	
	/*
	while(1){
		
		res = recv(dSC1, msg, sizeof(msg), 0) ;
		if (res<0){
			perror("Probleme de reception message 1");
			return -1;
		}
		printf("Message recu client 1: %s\n", msg) ;
		res = send(dSC2, msg, sizeof(msg), 0) ; 
		if (res<0){
			perror("Probleme de connection au serveur");
			return -1;
		}
		close (dSC) ; 
	}
	close (dS) ;
	*/
	return 0;
}

