
int CreationSocket();

void CreationServeur(int dS, char *port);

int ConnexionSocket(int dS);

//int EnvoiMsg(int dSC,char* msg);

int Envoi(int dSC,char * msg, int taille);

int Reception(int dSC,char * msg,int taille);

int FinConv(int dSC1,int dSC2,char * messfin);

int ConnexionServeur(int dS, char *port, char *IP);