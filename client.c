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
#include <json/json.h>


struct sockaddr_in serv;
int sock;
int conn, con;
char message[100]="";
int handshake = 0;
int fd;
struct ifreq ifr;
char empty[100] = "";

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
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	cip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
   
	inet_pton(AF_INET, sip, &serv.sin_addr);
	
	if(connect(sock, (struct sockaddr *)&serv, sizeof(serv)) == 0){
		printf("INGRESADO COMO USUARIO: %s\n", nickname);
		printf("SERVER IP: %s\n", sip);
		printf("CLIENTE IP: %s\n", cip);
		printf("test\n");
		

		json_object *juser = json_object_new_object();

		json_object *jHost = json_object_new_string(sip);
		json_object *jOrigin = json_object_new_string(cip);
		json_object *jUser = json_object_new_string(nickname);

		json_object_object_add(juser,"host",jHost);
		json_object_object_add(juser,"origin",jOrigin);
		json_object_object_add(juser,"user",jUser);
		
		const char *hand;
		hand = json_object_to_json_string(juser);
		send(sock, hand, strlen(hand),0);
		
		handshake = 0;
		strcpy(message,empty);
		while(handshake == 0){
			recv(sock, message, sizeof(message),0);
			printf("ESPERANDO CONEXION\n");
			printf("HANDSHAKE:%s\n", message);
			printf("CONEXION EXITOSA\n");
			if(message>0){
				handshake=1;
			}
			strcpy(message,empty);
			}
	}else{
		printf("CONEXION FALLIDA\n");
		exit(0);
	}
	
	int choice=0;
	while(choice!=5){

		printf("\n\tCLIENT MENU");
		printf("\n\t------------------------------");
		printf("\n\n\t 1. CHATEAR CON USUARIOS");
		printf("\n\t 2. LISTAR USUARIOS");
		printf("\n\t 3. CAMBIAR STATUS");
		printf("\n\t 4. AYUDA");
		printf("\n\t 5. SALIR");
		printf("\n\n INGRESE UNA OPCION: ");
		scanf("%d", &choice);
 
		switch(choice){
			case 1:
			printf("\nCHAT %c\n",1);
			json_object *jobj = json_object_new_object();
			char dest[50];
			printf("INGRESE DESTINATARIO: ");
			scanf("%s",dest);
			while(1){
			printf("INGRESE MENSAJE: ");
			strcpy(message,empty);
			scanf(" %[^\n]s",message);
			json_object *jAction = json_object_new_string("SEND_MESSAGE");
			json_object *jUser = json_object_new_string(nickname);
			json_object *jDest = json_object_new_string(dest);
			json_object *jMessage = json_object_new_string(message);
			json_object_object_add(jobj,"action",jAction);
			json_object_object_add(jobj,"from",jUser);
			json_object_object_add(jobj,"to",jDest);
			json_object_object_add(jobj,"message",jMessage);
			const char *theMessage;
			theMessage = json_object_to_json_string(jobj);
			send(sock, theMessage, sizeof(theMessage),0);
			strcpy(message,empty);
			}			
			break;
			
			case 2:
			printf("\nLISTA DE USUARIOS %c\n",2);
			
			json_object *jlist = json_object_new_object();
			json_object *jAction2 = json_object_new_string("LIST_USER");
			json_object_object_add(jlist,"action",jAction2);
			
			const char *listar;
			listar = json_object_to_json_string(jlist);
			send(sock, listar, strlen(listar),0);
			
			printf("\nBUSCAR USUARIO %c\n",2);
			json_object *jbus = json_object_new_object();
			char user[50];
			printf("INGRESE USUARIO: ");
			scanf("%s",user);

			json_object *jUser = json_object_new_string(nickname);

			json_object_object_add(jbus,"action",jAction2);
			json_object_object_add(jbus,"from",jUser);

			const char *bus;
			bus = json_object_to_json_string(jbus);
			send(sock, bus, strlen(bus),0);
			break;
			
			case 3:
			printf("\nCAMBIAR STATUS %c\n",3);
			json_object *jstatus = json_object_new_object();
			json_object *juser = json_object_new_object();
			json_object *jAction3 = json_object_new_string("CHANGE_STATUS");
			json_object *jId = json_object_new_string(nickname);
			json_object *jName = json_object_new_string(nickname);
			json_object *jStatus = json_object_new_string("busy");
			
			json_object_object_add(jstatus,"action",jAction3);
			json_object_object_add(juser,"id",jId);
			json_object_object_add(juser,"name",jName);
			json_object_object_add(juser,"status",jStatus);
  		  	json_object_object_add(jstatus,"user",juser);
			
			const char *change;
			change = json_object_to_json_string(jstatus);
			send(sock, change, strlen(change),0);
			break;
			
			case 4:
			printf("\nAYUDA %c\n",4);
			break;
			
			case 5:
			printf("\nSALIR %c\n",5);
			const char *bye;
			bye = "BYE";
			send(sock, bye, sizeof(bye),0);
			
			handshake = 0;
			strcpy(message,empty);
			while(handshake == 0){
				printf("CLIENTE DICE:%s\n", bye);
				recv(sock, message, sizeof(message),0);
				printf("SERVER DICE:%s\n", message);
				if(message>0){
					handshake=1;
				}
				strcpy(message,empty);
				}
			exit(0);
			
			default:
			printf("\nOPCION INVALIDA\n");
			choice=0;
			break;
		}
	}

}
