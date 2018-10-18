#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <ifaddrs.h>


struct sockaddr_in serv;
int sock;
int con = 1;
char message[100]="";

int fd;
struct ifreq ifr;

int main(int argc, char* argv[]) {
	
	char* nickname = argv[1];
	int cport = atoi(argv[2]);
	char* sip = argv[3];
	int sport = atoi(argv[4]);
	char* cip;


	sock = socket(AF_INET, SOCK_STREAM, 0);
	serv.sin_family=AF_INET;
	serv.sin_port = htons(sport);


	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "en0", IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	cip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
   
	inet_pton(AF_INET, sip, &serv.sin_addr);
	connect(sock, (struct sockaddr *)&serv, sizeof(serv));

	if(con = 1){
		printf("INGRESADO COMO USUARIO :%s\n", nickname);
		printf("SERVER IP :%s\n", sip);
		printf("CLIENTE IP :%s\n", cip);
	
	}

	int choice=0;
	while(choice!=4){
		clrscr();
		printf("\n\tCLIENT MENU");
		printf("\n\t------------------------------");
		printf("\n\n\t 1. CHATEAR CON USUARIOS");
		printf("\n\t 2. LISTAR USUARIOS");
		printf("\n\t 3. AYUDA");
		printf("\n\t 4. SALIR");
		printf("\n\n INGRESE UNA OPCION: ");
		scanf("%d", &choice);
 
		switch(choice){
			case 1:
			printf("\nCHATEAR CON USUARIOS %c",1);
			break;
			case 2:
			printf("\nLISTAR USUARIOS %c",2);
			break;
			case 3:
			printf("\nAYUDA %c",3);
			break;
			case 4:
			printf("\nSALIR %c",4);
			exit(0);
			otherwise:
			printf("\nOPCION INVALIDA");
		}
		getch();
	}

}