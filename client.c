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
#include <json.h>


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

	if(con == 1){
		printf("INGRESADO COMO USUARIO :%s\n", nickname);
		printf("SERVER IP :%s\n", sip);
		printf("CLIENTE IP :%s\n", cip);
		
		json_object *juser = json_object_new_object();

		json_object *jHost = json_object_new_string(host);
		json_object *jOrigin = json_object_new_string(cip);
		json_object *jUser = json_object_new_string(nickname);

		json_object_object_add(juser,"host",jHost);
		json_object_object_add(juser,"origin",jOrigin);
		json_object_object_add(juser,"user",jUser);
		
		char *mes;
		mes = json_object_to_json_string(juser);
		send(sock, mes, strlen(mes),0);
	}

	int choice=0;
	while(choice!=4){

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
			printf("\nCHAT %c",1);
			json_object *jobj = json_object_new_object();
			char dest[50];
			printf("destinatario:");
			scanf("%s",dest);
			while(1){
			printf("Enter a message:");
			scanf(" %[^\n]s",message);
			json_object *jAction = json_object_new_string("SEND_MESSAGE");
			json_object *jUser = json_object_new_string(nickname);
			json_object *jDest = json_object_new_string(dest);
			json_object *jMessage = json_object_new_string(message);
			json_object_object_add(jobj,"action",jAction);
			json_object_object_add(jobj,"from",jUser);
			json_object_object_add(jobj,"to",jDest);
			json_object_object_add(jobj,"message",jMessage);
			char *theMessage;
			theMessage = json_object_to_json_string(jobj);
			send(fd, theMessage, strlen(theMessage),0);
			}			
			break;
			
			case 2:
			printf("\nLISTA DE USUARIOS %c",2);
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
	}

}
