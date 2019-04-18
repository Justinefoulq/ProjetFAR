#define CMAX 100

int CreationSocket();

int CreationServeur(int dS, char *port);

int ConnexionSocket(int dS);

int Envoi(int dSC,char * msg, int taille);

int Reception(int dSC,char * msg,int taille);

int FinConv(int dSC1,int dSC2);

int ConnexionServeur(int dS, char *port, char *IP);

int FinGroupe(int dSC[CMAX],int nbC);