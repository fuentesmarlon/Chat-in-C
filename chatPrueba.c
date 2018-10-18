#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <json.h>

struct sockaddr_in serv;
int fd;
int conn;
char message[100]="";

int main(){
fd = socket(AF_INET, SOCK_STREAM, 0);
serv.sin_family=AF_INET;
serv.sin_port = htons(8096);
inet_pton(AF_INET,"127.0.0.1", &serv.sin_addr);
connect(fd, (struct sockaddr *)&serv, sizeof(serv));
//PArams del usuario 
char name[50];
int choice;
printf("\nBienvenido al Server");
printf("\nIngrese su nombre de usuario:\n\t");
scanf(" %s",name);

/*while(choice !=6){
printf("\n\t 1. Chat Grupal");
printf("\n\t 2. Chat Privado");
printf("\n\t 3. Cambio de Estado");
printf("\n\t 4. Ver usuarios disponibles");
printf("\n\t 5. Salir");
printf("\n\t Ingrese su eleccion:\n\t");
scanf("%d",&choice);
if(choice == 1){printf("chat grupal");}
if(choice == 2){*/
json_object *jobj = json_object_new_object();
char dest[50];
printf("destinatario:");
scanf("%s",dest);
//while(1){
	printf("Enter a message:");
	//fgets(message,100,stdin);
	scanf("%s",message);
	json_object *jAction = json_object_new_string("SEND_MESSAGE");
	json_object *jUser = json_object_new_string(name);
	json_object *jDest = json_object_new_string(dest);
	json_object *jMessage = json_object_new_string(message);
	json_object_object_add(jobj,"action",jAction);
	json_object_object_add(jobj,"from",jUser);
	json_object_object_add(jobj,"to",jDest);
	json_object_object_add(jobj,"message",jMessage);
	char *theMessage;
	theMessage = json_object_to_json_string(jobj);
	//printf("%s",theMessage);

	send(fd, theMessage, strlen(theMessage),0);
	
//}
//}
/*if(choice == 3){
printf("\n\t chat privado");
}
if(choice == 4){
printf("\n\t cambio de estado");
}
if(choice == 5){
printf("\n\t cambio de estado");
exit(0);
}*/
/*
else{printf("ELija una opcion valida");}

}*/


}
