#define NMAX 100


int CreationSocket();

void CreationServeur(int dS, char *port);

int ConnectionSocket(int dS);

void Envoi(int dSC,char msg[NMAX], int taille);

void Reception(int dSC,char msg[NMAX], int taille);

int FinConv(int dSC1,int dSC2,char messfin[NMAX], int taille);

void ConnectionServeur(int dS, char *port, char *IP);