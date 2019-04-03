#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define NMAX 100

int main(int argc, char ** argv){
	//Creation Socket
	int dSock = socket(PF_INET, SOCK_STREAM, 0);
	if (dSock < 0 ){
		perror("Probleme creation socket");
		return -1;
	}
	//Connection au serveur
	struct sockaddr_in adServ ;
	adServ.sin_family = AF_INET ; 
	adServ.sin_port = htons((short)atoi(argv[1])) ;
	int res = inet_pton(AF_INET, argv[2], &(adServ.sin_addr)) ; 
	socklen_t lgA = sizeof(struct sockaddr_in) ;
	res = connect(dSock, (struct sockaddr *) &adServ, lgA) ; 
	if (res<0){
		perror("Probleme de connection au serveur");
		return -1;
	}
	char msg[NMAX];
	char mot[NMAX];

	res = recv(dSock, msg, sizeof(msg), 0) ;
	if (res<0){
		perror("Probleme de reception message 1");
		return -1;
	}
	printf("%s\n",msg);

	while(1){
		res = recv(dSock, msg, sizeof(msg), 0) ;
		if (res<0){
			perror("Probleme de reception message 1");
			return -1;
		}
		printf("%s",msg);
		printf("					");
		fgets(mot,NMAX,stdin);
		res = send(dSock,mot,NMAX,0);
		if (res<0){
			perror("Message pas envoyé");
			return -1;
		}
	}
	

	return 0;
}
