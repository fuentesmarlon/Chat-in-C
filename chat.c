//nos basamos en el siguiente código: https://medium.com/@yashitmaheshwary/simple-chat-server-using-sockets-in-c-f72fc8b5b24e

#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <json/json.h>
#include <arpa/inet.h>
struct sockaddr_in serv;
int fd;
int conn;
int numUsers;
char empty[100] = "";

struct Usuario {
	int id;
	char *user;
	char *status;
	char *ip;
	int socket;
};

struct Usuario usuarios[5];

int main(int argc, char * argv[]){
	int port = atoi(argv[0]);
	numUsers = 0;
	char message[100] ="";

	serv.sin_family = AF_INET;
	serv.sin_port=htons(port);
	serv.sin_addr.s_addr = INADDR_ANY;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	bind(fd, (struct sockaddr *)&serv, sizeof(serv));
	listen(fd,10);

	while(conn = accept(fd, (struct sockaddr *)NULL, NULL))
	{
		printf("esperando");
		int pid;
		if((pid=fork())==0){
			while(recv(conn,message,100,0)>0){
				printf("Mensaje:%s\n", message);
				json_object * jobj = json_tokener_parse(message);
				int exito = parseConnection(jobj, conn);
				if(exito == 0){
					break;				
				}
				else{
					strcpy(message,empty);
					while(recv(conn,message,100,0)>0){
						json_object *jobj = json_tokener_parse(message);
						json_object *action;
						json_object_object_get_ex(jobj,"action",&action);
						const char *accion = json_object_get_string(action);
						if(accion == "SEND_MESSAGE"){
							send_message(jobj);
						}
						if(recv(conn,message,100,0)==0){
							break;
						}
					}
					break;
				}		
				
			}
		exit(0);
		}
	}
	
}

int parseConnection(json_object * jobj, int socket){
	
	json_object* origin;
	json_object* nombre;

	json_object_object_get_ex(jobj,"origin",&origin);
	const char *originS = json_object_get_string(origin);
	json_object_object_get_ex(jobj,"nombre",&nombre);
	const char *nombreS = json_object_get_string(nombre);

	int c;
	int yasta = 0;
	if(numUsers > 0){
		for(c=0;c<5;c++){
			if(strcmp(nombreS,usuarios[c].user) == 0){
				yasta = 1;		
				break;
			}	
		}
	}
	if(yasta){
		sendMensajeError(originS, socket);
		return 0;
	}
	else{
		agregarUsuario(originS, nombreS, socket);
		return 1;
	}
}

int send_message(json_object * jobj){
	json_object *from;
	json_object *to;
	json_object *message;

	json_object_object_get_ex(jobj,"from",&from);
	json_object_object_get_ex(jobj,"to",&to);
	json_object_object_get_ex(jobj,"message",&message);

	const char *toS = json_object_get_string(to);
	
	struct json_object *action = json_object_new_string("RECEIVE_MESSAGE");
	struct json_object *reply = json_object_new_object();
	int socket = 0;
	int c;	
	for(c=0;c<numUsers;c++){
		if(usuarios[c].id == (int)toS){
			socket = usuarios[c].socket;
			break;
		}
	}
	json_object_object_add(reply,"action",action);
	json_object_object_add(reply,"from",from);
	json_object_object_add(reply,"to",to);
	json_object_object_add(reply,"message",message);
	send(socket,json_object_get_string(reply),100,0);
}

int sendMensajeError(char *origin, int socket){
	char *mensaje = "{\"status\": \"ERROR\", \"message\": \"Usuario ingresado ya existe.\"}";
	send(socket, mensaje, 100, 0);
	return 0;
}

int agregarUsuario(char *origin, char *nombre, int socket){
	usuarios[numUsers].id = numUsers;
	usuarios[numUsers].user = nombre;
	usuarios[numUsers].status = "active";
	usuarios[numUsers].ip = origin;
	usuarios[numUsers].socket = socket;
	char *idS;
	sprintf(idS, "%d", numUsers);
	struct json_object *id = json_object_new_string(idS);
	struct json_object *name = json_object_new_string(nombre);
	struct json_object *status = json_object_new_string("activo");
	struct json_object *user = json_object_new_object();
	json_object_object_add(user,"id",id);
	json_object_object_add(user,"name",name);	
	json_object_object_add(user,"status",status);
	struct json_object *reply = json_object_new_object();
	struct json_object *ok = json_object_new_string("OK");
	json_object_object_add(reply,"status",ok);
	json_object_object_add(reply,"user",user);
	send(socket, json_object_get_string(reply),100,0);
	numUsers++;
	return 0;
}
