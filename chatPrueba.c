#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <json/json.h>

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
char stats[50];
int choice;
printf("\n\tBienvenido al Server");
printf("\n\tIngrese su nombre de usuario:\n\t");
scanf(" %s",name);

while(choice !=6){
printf("\n\t 1. Chat Grupal");
printf("\n\t 2. Chat Privado");
printf("\n\t 3. Cambio de Estado");
printf("\n\t 4. Ver usuarios disponibles");
printf("\n\t 5. Salir");
printf("\n\t Ingrese su eleccion:\n\t");
scanf("%d",&choice);
if(choice == 1){
json_object *jobj = json_object_new_object();

while(1){
	printf("Enter a message:");
	fgets(message,100,stdin);
	printf("%s",message);
	send(fd, message, strlen(message),0);
	
}
}
if(choice == 2){
printf("\n\t chat privado");
}
if(choice == 3){
printf("\n\t cambio de estado");
}
if(choice == 4){
printf("\n\t cambio de estado");
}
if(choice == 5){
printf("\n\t salir");
exit(0);
}
else{printf("ELija una opcion valida");}

}


}
