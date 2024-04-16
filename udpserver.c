/*

   Conexion UDP
   Codigo del servidor

   Nombre Archivo: udpserver.c   
   Fecha: Febrero 2023

   Compilacion: cc udpserver.c -lnsl -o udpserver
   Ejecución: ./udpserver

*/

// server program for udp connection 
#include <stdio.h> 
#include <string.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h> 
#include <stdlib.h>

#define PORT 5003 
#define MAXLINE 1000 


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
  
// Driver code 
int main() 
{    
    char buffer[MAXLINE]; 
    char *message = "Hello Client"; 
    int listenfd, len; 
    struct sockaddr_in servaddr, cliaddr; 
    int empezar;
    printf("Listening in port number: %d", PORT);
    //printf("\nPresione cualquier tecla para empezar \n");
    //scanf("%d",&empezar);

    //bzero(&servaddr, sizeof(servaddr)); 
  
    // Create a UDP Socket 
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(listenfd == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }         

    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_family = AF_INET;  

    // bind server address to socket descriptor 
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
       
    //receive the datagram 
    len = sizeof(cliaddr); 
    int n = recvfrom(listenfd, buffer, MAXLINE, 
            0, (struct sockaddr*)&cliaddr,&len); //receive message from server 
    if(n < 0) {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }else{
        buffer[n] = '\0'; 
        printf("\nHe recibido del cliente: ");
        printf("%s\n",buffer);
    }       
    // send the response 
    sendto(listenfd, message, strlen(message), 0, 
          (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
    close(listenfd);
    printf("Conexion cerrada\n");
    return 0;
} 
