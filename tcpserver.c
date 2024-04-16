/*
   Codigo del servidor

   Nombre Archivo: tcpserver.c   

   Compilacion: cc tcpserver.c -lnsl -o tcpserver
   Ejecución: ./tcpserver
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
/* The following headers was required in old or some compilers*/
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>	// it is required to call signal handler functions
#include <unistd.h>  // it is required to close the socket descriptor

#define  jsonSIZE  10000
#define  msgSIZE   2048      /* longitud maxima parametro entrada/salida */
#define  PUERTO    5000	     /* numero puerto arbitrario */

int                  sd, sd_actual, sd_actual2;  /* descriptores de sockets */
int                  addrlen;        /* longitud msgecciones */
struct sockaddr_in   sind, pin;      /* msgecciones sockets cliente u servidor */


/*  procedimiento de aborte del servidor, si llega una senal SIGINT */
/* ( <ctrl> <c> ) se cierra el socket y se aborta el programa       */
void aborta_handler(int sig){
   printf("....abortando el proceso servidor %d\n",sig);
   close(sd);  
   close(sd_actual); 
   exit(1);
}


void nJsonFile(char *json,char* inst, int id, int fila, int col){
	char ejemplo[1000]="";
	char nstr[10];
	strcpy(ejemplo,"{\"inst\":\"");
	strcat(ejemplo,inst);
	strcat(ejemplo,"\",\"id\": ");
	sprintf(nstr, "%d", id);
	strcat(ejemplo,nstr);
	strcat(ejemplo," ,\"fila\": ");
	sprintf(nstr, "%d", fila);
	strcat(ejemplo,nstr);
	strcat(ejemplo,",\"columna\": ");
	sprintf(nstr, "%d", col);
	strcat(ejemplo,nstr);
	strcat(ejemplo,"}");
	strcpy(json,ejemplo);
}


void nJsonTurno(char *json,int id){
	char ejemplo[1000]="";
	char nstr[10];
	strcpy(ejemplo,"{\"inst\":\"");
	strcat(ejemplo,inst);
	strcat(ejemplo,"\",\"id\": ");
	sprintf(nstr, "%d", id);
	strcat(ejemplo,nstr);
	strcat(ejemplo,"}");
	strcpy(json,ejemplo);
}

int isNumber(const char *str) {
    if (!str || *str == '\0') // Null or empty string
        return 0;

    char *endptr;
    errno = 0; // To distinguish success/failure after call
    strtol(str, &endptr, 10);

    // Check for various possible errors
    if ((errno == ERANGE && (strtol(str, NULL, 10) == LONG_MAX || strtol(str, NULL, 10) == LONG_MIN)) || (errno != 0 && strtol(str, NULL, 10) == 0)) {
        return 0; // Not a valid number
    }
    // If there are any non-numeric characters in the string, it's not a number
    while (*endptr != '\0') {
        if (!isspace((unsigned char)*endptr) && !isdigit((unsigned char)*endptr)) {
            	return 0; // Not a valid number
        }
        endptr++;
    }
    return 1; // String can be converted to a number
}

void readJsonFile(char *archivo, char *inst, int *id, int *fila, int *col){
	char izq[100], der[100];
	strcpy(izq,"");
	strcpy(der,"");
	int it=0,ap=0, lookl=0;
	*id=0;
	*fila=0;
	*col=0;
	while(archivo[it]!='\0'){
		if(archivo[it]=='\"'){
			if(ap==0){
				ap=1;
				if(lookl==0){
					strcpy(izq,"");
				} else{
					strcpy(der,"");
				}
			} else{
				ap=0;
			}
		}
		if(ap==1){
			
				if(lookl==0){
					strcat(izq,&archivo[it]);
				} else{
					strcat(der,&archivo[it]);
				}
		}else{
			if(archivo[it]==':'){
				lookl=1;
			}else{
				if(archivo[it]!=','){
					if(strcmp(izq,"inst")==0)strcpy(inst,der);
					if(strcmp(izq,"id")==0&&isNumber(der))*id=atoi(der);
					if(strcmp(izq,"fila")==0&&isNumber(der))*fila=atoi(der);
					if(strcmp(izq,"columna")==0&&isNumber(der))*col=atoi(der);
					lookl=0;
					strcpy(der,"");
					strcpy(izq,"");
				}
			}
		}
		it++;
	}
	
	
}

  	int tablero[7][7];
int won(){
	for( int i=0; i<7; i++){
		for( int k=0; k<4; k++){
			if(tablero[i][k]==tablero[i][k+1]&&tablero[i][k]==tablero[i][k+2]&&tablero[i][k]==tablero[i][k+3])return 1;
		}
	}
	for( int i=0; i<7; i++){
		for( int k=0; k<4; k++){
			if(tablero[k][i]==tablero[k+1][i]&&tablero[k][i]==tablero[k+2][i]&&tablero[k][i]==tablero[k+3][i])return 1;
		}
	}
	for( int i=0; i<4; i++){
		for( int k=0; k<4; k++){
			if(tablero[k][i]==tablero[k+1][i+1]&&tablero[k][i]==tablero[k+2][i+2]&&tablero[k][i]==tablero[k+3][i+3])return 1;
		}
	}
	for( int i=6; i>2; i++){
		for( int k=0; k<4; k++){
			if(tablero[k][i]==tablero[k+1][i-1]&&tablero[k][i]==tablero[k+2][i-2]&&tablero[k][i]==tablero[k+3][i-3])return 1;
		}
	}
	return 0;
}
int udp(char *message)
{    
    char buffer[1000]; 
    char ip[16]="172.18.2.5";
    int sockfd; 
    int port=5003;
    struct sockaddr_in servaddr; 
      
    // clear servaddr 
    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sin_addr.s_addr = inet_addr(ip); 
    servaddr.sin_port = htons(port); 
    servaddr.sin_family = AF_INET; 
      
    // create datagram socket 
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    if(sockfd==-1){
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    //it is not required to establish a connection
    //directly sending a message
    int r = sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
    if(r==-1){
        perror("sendto failed");
        exit(EXIT_FAILURE);
    }
      
    // waiting for response 
    int len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr*)&servaddr, &len);
    if(n < 0) {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }else{
        buffer[n] = '\0'; 
        printf("\nHe recibido del server: ");
        printf("%s\n",buffer);
    }
    // close the descriptor 
    close(sockfd); 
    printf("Conexion cerrada\n");
} 




int main(){
	char  msg[msgSIZE];	     /* parametro entrada y salida */
	char  json[jsonSIZE];	     /* parametro entrada y salida */

	/*
	When the user presses <Ctrl + C>, the aborta_handler function will be called, 
	and such a message will be printed. 
	Note that the signal function returns SIG_ERR if it is unable to set the 
	signal handler, executing line 54.
	*/	
   if(signal(SIGINT, aborta_handler) == SIG_ERR){
   	perror("Could not set signal handler");
      return 1;
   }
       //signal(SIGINT, aborta);      /* activando la senal SIGINT */

/* obtencion de un socket tipo internet */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

/* asignar msgecciones en la estructura de msgecciones */
	sind.sin_family = AF_INET;
	sind.sin_addr.s_addr = INADDR_ANY;   /* INADDR_ANY=0x000000 = yo mismo */
	sind.sin_port = htons(PUERTO);       /*  convirtiendo a formato red */

/* asociando el socket al numero de puerto */
	if (bind(sd, (struct sockaddr *)&sind, sizeof(sind)) == -1) {
		perror("bind");
		exit(1);
	}

/* ponerse a escuchar a traves del socket */
	if (listen(sd, 5) == -1) {
		perror("listen");
		exit(1);
	}

	/* esperando que un cliente solicite un servicio */
	if ((sd_actual = accept(sd, (struct sockaddr *)&pin, &addrlen)) == -1) {
		perror("accept");
		exit(1);
	}
	if ((sd_actual2 = accept(sd, (struct sockaddr *)&pin, &addrlen)) == -1) {
		perror("accept");
		exit(1);
	}
	printf("Conexiones establecidas\n");
	for( int i=0; 0<7;i++){
		for( int j=0; j<7; j++){
			tablero[i][j]=0;
		}
	}
	printf("sd_Actual: %d\n",(int)sd_actual );
	char sigue='S';
	char msgReceived[1000];
	char inst[100];
	int turno=0;
	int idc, fil, col;
	strcpy(json,"");
	while(sigue=='S'){				
		/* tomar un mensaje del cliente */
		int n;
		if(turno ==0){			
			n = recv(sd_actual, msg, sizeof(msg), 0);
			turno=1;
		}
		else{			
			n = recv(sd_actual2, msg, sizeof(msg), 0);
			turno=0;
		}
		if (n == -1) {
			perror("recv");
			exit(1);
		}		
		msg[n] = '\0';		
		printf("Client %d sent: %s\n", 1-turno, msg);
		strcpy(json,msg);
		readJsonFile(json, inst, &idc, &fil, &col);
		if((strcmp(inst,"close")==0)){ //it means that the conversation must be closed
			sigue='N';
			nJsonFile(json,inst,idc,fil,col);
		}else{
			if(strcmp(inst,"LOGIN")==0||strcmp(inst,"REGISTER")==0){
				udp(json);
			} else{		
				if(strcmp(inst,"ADD_FICHA")==0){
					int nfil=0;
					while(nfil<7&&tablero[col][nfil]!=0)nfil++;
					fil=nfil;
					tablero[col][fil]=idc;
				}
				if(won(tablero))
					nJsonFile(json,"GANA",idc,fil,col);
				else
					nJsonFile(json,"ADD_FICHA",idc,fil,col);
			}
		}		
		/* enviando la respuesta del servicio */
		int sent;
		if(turno ==0){
			if ( sent = send(sd_actual, json, strlen(json), 0) == -1) {
				perror("send");
				exit(1);
			}
		} else{
			if ( sent = send(sd_actual2, json, strlen(json), 0) == -1) {
				perror("send");
				exit(1);
			}
			
		}
	}

/* cerrar los dos sockets */
	close(sd_actual);  
   close(sd);
   printf("Conexion cerrada\n");
	return 0;
}
