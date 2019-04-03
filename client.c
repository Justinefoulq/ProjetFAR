#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#define NMAX 20

int main(int argc, char ** argv){
	int dSock = socket(PF_INET, SOCK_STREAM, 0);
	if (dSock < 0 ){
		perror("Probleme creation socket");
		return -1;
	}
	struct sockaddr_in adServ ;
	adServ.sin_family = AF_INET ; 
	adServ.sin_port = htons((short)atoi(argv[1])) ;
	int res = inet_pton(AF_INET, argv[2], &(adServ.sin_addr)) ; 
	socklen_t lgA = sizeof(struct sockaddr_in) ;
	res = connect(dSock, (struct sockaddr *) &adServ, lgA) ; 
	if (res<0){
		perror("Probleme de connection au serveur");
		return -2;
	}
	char mot[NMAX];
	printf("Que voulez vous envoyer ?\n");
	fgets(mot,NMAX,stdin);
	res = send(dSock,mot,NMAX,0);
	if (res<0){
		perror("Message pas envoyé");
		return -3;
	}
	printf("%d\n",res);
	return 0;
}
