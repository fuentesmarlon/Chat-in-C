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
#include <pthread.h>


pthread_t thread;
struct sockaddr_in serv;
int sock;
int conn, con;
char message[100]="";
int handshake = 0;
int fd;
struct ifreq ifr;
char empty[100] = "";

void * listening(){
	char meServer[1024];
	int resp;
	while(1){
		bzero(meServer, 1024);
		resp = recv(fd,meServer,1024,0);
		if(resp<=0){
			
		}else{
			json_object *jChecker = json_tokener_parse(meServer);
			json_object *action;
			json_object_object_get_ex(jChecker,"action",&action);
			const char *accion = json_object_get_string(action);
			
			if(accion=="RECEIVE_MESSAGE"){
				json_object *from;
				json_object *messageJ;
				json_object *jReceive =json_tokener_parse(meServer);
				json_object_object_get_ex(jReceive,"from",&from);
				json_object_object_get_ex(jReceive,"message",&messageJ);
				const char *fromName = json_object_get_string(from);
				const char *meString = json_object_get_string(messageJ);
				printf("MENSAJE DE: %s\n",fromName);
				printf("MENSAJE: %s\n",meString);		

			}if(accion=="LIST_USER"){
				json_object *users;
				json_object *jList = json_tokener_parse(meServer);
				json_object_object_get_ex(jList,"users",&users);
				const char *listUsers = json_object_get_string(jList);
				printf("LISTA DE USUARIOS: %s",listUsers);

			}if(accion=="CHANGED_STATUS"){
				json_object *user;
				json_object *jStatus = json_tokener_parse(meServer);
				json_object_object_get_ex(jStatus,"user",&user);
				const char *changedStatus = json_object_get_string(jStatus);
				printf("STATUS CAMBIO A: %s",changedStatus);

			}if(accion=="USER_DISCONNECTED"){
				json_object *user;
				json_object *jStatus = json_tokener_parse(meServer);
				json_object_object_get_ex(jStatus,"user",&user);
				const char *changedStatus = json_object_get_string(jStatus);
				printf("STATUS CAMBIO A: %s",changedStatus);
				

			}else{
				printf("ACTION ERROR");
			}

		}

	}
}

int main(int argc, char* argv[]) {
	
	char* nickname = argv[1];
	int cport = atoi(argv[2]);
	char* sip = argv[3];
	int sport = atoi(argv[4]);
	char* cip;
	const char *id;


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
		printf("INGRESADO COMO USUARIO: %s\n\n", nickname);
		printf("SERVER IP: %s\n", sip);
		printf("CLIENTE IP: %s\n\n", cip);


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
		bzero(message, 100);
		while(handshake == 0){
			recv(sock, message, sizeof(message),0);
			printf("ESPERANDO CONEXION\n\n");
			printf("HANDSHAKE ");
			
			json_object *jhand = json_tokener_parse(message);
			json_object *jshake;
			json_object *shake;
			json_object *usrid;
			json_object *usrname;
			json_object *usrstatus;
			json_object_object_get_ex(jhand,"status",&shake);
			json_object_object_get_ex(jhand,"user",&jshake);
			json_object_object_get_ex(jshake,"id",&usrid);
			json_object_object_get_ex(jshake,"name",&usrname);
			json_object_object_get_ex(jshake,"status",&usrstatus);
			const char *ok = json_object_get_string(shake);
			id = json_object_get_string(usrid);
			const char *uname = json_object_get_string(usrname);
			const char *ust = json_object_get_string(usrstatus);

			printf("STATUS %s\n\n", ok);
			printf("ID: %s\n", id);
			printf("USER: %s\n", uname);
			printf("STATUS: %s\n", ust);
			printf("\nCONEXION EXITOSA\n");
			
			if(message>0){
				handshake=1;
			}
			bzero(message, 100);
			}
	}else{
		printf("CONEXION FALLIDA\n\n");
		exit(0);
	}
	
	int choice=0;
	pthread_create(&thread,NULL,listening,(void*)&sock);
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
			printf("\nCHAT \n",1);
			json_object *jobj = json_object_new_object();
			char dest[50];
			printf("INGRESE DESTINATARIO: ");
			scanf("%s",dest);
			while(1){
			printf("INGRESE MENSAJE: ");
			bzero(message, 100);
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
 		    bzero(message, 100);
			}			
			break;
			
			case 2:
			printf("\nLISTA DE USUARIOS \n", 2);
			
			json_object *jlist = json_object_new_object();
			json_object *jAction2 = json_object_new_string("LIST_USER");
			json_object_object_add(jlist,"action",jAction2);
			const char *listar;
			
			listar = json_object_to_json_string(jlist);
			send(sock, listar, strlen(listar),0);
			
			handshake = 0;
			bzero(message, 100);
			while(handshake == 0){
				printf("\nESPERANDO LISTA...\n");
				recv(sock, message, sizeof(message),0);
				printf("LISTA: %s\n", message);
				
				if(message>0){
					handshake=1;
				}
				bzero(message, 100);
			}
			
			printf("\nBUSCAR USUARIO? Y/N\n",2);
			char buser[5];
			scanf("%s",buser);
			if(!strcmp(buser, "y")) {
				json_object *jbus = json_object_new_object();
				char user[50];
				printf("INGRESE ID DE USUARIO: ");
				scanf("%s",user);

				json_object *jUser = json_object_new_string(user);

				json_object_object_add(jbus,"action",jAction2);
				json_object_object_add(jbus,"user",jUser);

				const char *bus;
				bus = json_object_to_json_string(jbus);
				send(sock, bus, sizeof(bus),0);
			
				handshake = 0;
				bzero(message, 100);
				while(handshake == 0){
					printf("ESPERANDO USUARIO...\n");
					recv(sock, message, strlen(message),0);
					printf("^^ %s\n", message);
					json_object *j1users = json_tokener_parse(message);
					json_object *j1user;
					json_object *j1id;
					json_object *j1name;
					json_object *j1status;
					json_object_object_get_ex(j1users,"users",&j1user);
					json_object_object_get_ex(j1user,"id",&j1id);
					json_object_object_get_ex(j1user,"name",&j1name);
					json_object_object_get_ex(j1user,"status",&j1status);
					const char *id1 = json_object_get_string(j1id);
					const char *name1 = json_object_get_string(j1name);
					const char *status1 = json_object_get_string(j1status);
				
				
					printf("INFO USUARIO: %s\n", name1);
					printf("ID: %s\n", id1);
					printf("STATUS: %s\n", status1);
					printf("\n\n");
				
					if(message>0){
						handshake=1;
					}
					bzero(message, 100);
				}
			}
			break;
			
			case 3:
			printf("\nCAMBIAR STATUS \n",3);
			
			json_object *jstatus = json_object_new_object();
			json_object *jAction3 = json_object_new_string("CHANGE_STATUS");
			json_object *jId = json_object_new_string(nickname);
			
			int status=0;
			char *st = "active";
			while(status!=4){
				printf("\n\t------------------------------");
				printf("\n\n\t 1. ACTIVE");
				printf("\n\t 2. BUSY");
				printf("\n\t 3. INACTIVE");
				printf("\n\n INGRESE UNA OPCION: ");
				scanf("%d", &status);
				switch(status){
					case 1:
					st = "active";
					status=4;
					break;
					
					case 2:
					st = "busy";
					status=4;
					break;
					
					case 3:
					st = "inactive";
					status=4;
					break;
					
					default:
					printf("\nOPCION INVALIDA\n");
					status=0;
					break;
				}
			}
			
			json_object *jStatus = json_object_new_string(st);
			
			json_object_object_add(jstatus,"action",jAction3);
			json_object_object_add(jstatus,"user",jId);
  		  	json_object_object_add(jstatus,"status",jStatus);
			
			const char *change;
			change = json_object_to_json_string(jstatus);
			send(sock, change, strlen(change),0);
			break;
			
			case 4:
			printf("\nAYUDA \n",4);
			printf("\n\t------------------------------");
			printf("\n\n\t 1. CHATEAR CON USUARIOS CONECTADOS");
			printf("\n\t 2. LISTAR USUARIOS CONECTADOS");
			printf("\n\t 3. CAMBIAR STATUS DE USUARIO");
			printf("\n\t 4. VER ESTA AYUDA");
			printf("\n\t 5. SALIR DEL CHAT ");
			printf("\n\t------------------------------\n\n\n");
			break;
			
			case 5:
			printf("\nSALIR \n",5);
			const char *bye;
			bye = "BYE";
			send(sock, bye, sizeof(bye),0);
			
			handshake = 0;
			bzero(message, 100);
			while(handshake == 0){
				printf("CLIENTE DICE: %s\n", bye);
				recv(sock, message, sizeof(message),0);
				printf("SERVER DICE: %s\n", message);
				if(message>0){
					handshake=1;
				}
				bzero(message, 100);
				}
			exit(0);
			
			default:
			printf("\nOPCION INVALIDA\n");
			choice=0;
			break;
		}
	}

}
