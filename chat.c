//nos basamos en el siguiente código: https://medium.com/@yashitmaheshwary/simple-chat-server-using-sockets-in-c-f72fc8b5b24e
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <json/json.h>
#include <arpa/inet.h>
#include <pthread.h>


struct sockaddr_in serv, cli;

int numUsers =0;
char empty[8000] = "";
void* server(void*);

struct Usuario {
	int id;
	char *user;
	char *status;
	char *ip;
	int socket;
};

struct Usuario usuarios[5];

int main(int argc, char * argv[]){
	
	struct sockaddr_in servidor, cliente;
	int conn,fd;
	int no = 0;
	int port = atoi(argv[1]);
	printf("%d\n",port);
	char message[8000] ="";
	char empty[8000] ="";
	socklen_t c;

	memset(&servidor, 0, sizeof(struct sockaddr_in));
	memset(&cliente, 0, sizeof(struct sockaddr_in));

	pthread_t thread;

	serv.sin_family = AF_INET;
	serv.sin_port=htons(port);
	serv.sin_addr.s_addr = INADDR_ANY;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	bind(fd, (struct sockaddr *)&serv, sizeof(serv));
	listen(fd,10);
	int clilen = sizeof(struct sockaddr_in);

	while(conn = accept(fd, (struct sockaddr *)&cliente, &clilen)){
		printf("conexion aceptada\n");

		if (pthread_create(&thread, NULL, server, (void*)&conn) != 0){
			printf("No se pudo crear un nuevo thread");
		}
		/*int pid;
		if((pid=fork())==0){
			while(recv(conn[no],message,100,0)>0){
				
			}
			printf("saliendo del segundo ciclo\n");
			exit(0);
		}*/
	}
	close(conn);
	close(fd);
	return(0);	
}

void* server(void* sock){
	int newsockfd=*(int*) sock;
	
	puts("escuchando1");
	char message[8000];
	char empty[8000] ="";
	bzero(message,256);
	recv(newsockfd,message,sizeof(message),0);
	
	printf("Mensaje:%s\n", message);
	json_object * jobj = json_tokener_parse(message);
	printf("se creo el jobj\n");
	int exito = parseConnection(jobj, newsockfd);
	printf("imprimiendo usuarios %d\n",numUsers);
	printf("se parseo la conexion\n");
	strcpy(message,empty);
	if(exito == 1){
		printf("exito == 1\n");
		strcpy(message,empty);
		printf(message);
		printf("antes de entrar al while\n");
		
		recv(newsockfd,message,8000,0);
		printf("Mensaje:%s\n", message);
		if(strcmp(message,"BYE")==0){
			printf("se cerro la comunicacion\n");
			send(newsockfd,"BYE",sizeof("BYE"),0);
			goto END;
		}
		json_object *jobj = json_tokener_parse(message);
		json_object *action;
		json_object_object_get_ex(jobj,"action",&action);
		printf("accion jalada\n");
		const char *accion = json_object_get_string(action);
		printf("%s\n",accion);
		if(strcmp(accion,"SEND_MESSAGE")==0){
			printf("enviando mensaje\n");
			send_message(jobj);
		}
		else if(strcmp(accion,"LIST_USER")==0){
			json_object *user;
			json_bool booleano;
			booleano = json_object_object_get_ex(jobj,"user",&user);
			if (booleano){
				printf("devolviendo un solo usuario\n");
				devolver_usuario(jobj,user,newsockfd);
			}
			else{
				printf("listando usuarios\n");
				listar_usuarios(jobj,newsockfd);
			}
			
		}
		else if(strcmp(accion,"CHANGE_STATUS")==0){
			printf("cambiando estado\n");
			cambiarEstado(jobj);							
		}
		else{
			printf("se cerro la comunicacion\n");
			send(newsockfd,"BYE",sizeof("BYE"),0);
			goto END;
		}
		strcpy(message,empty);
	}
	else{
		printf("exito != 1");	
	}
	END:
	printf("salio del primer ciclo\n");
	close(newsockfd);
	pthread_exit(NULL);
}
int parseConnection(json_object * jobj, int socket){
	printf("imprimiendo usuarios %d\n",numUsers);
	
	json_object* origin;
	json_object* nombre;
	printf("se entro el metodo\n");
	json_object_object_get_ex(jobj,"origin",&origin);
	const char *originS = json_object_get_string(origin);
	printf("origen:%s\n",originS);
	json_object_object_get_ex(jobj,"user",&nombre);
	const char *nombreS = json_object_get_string(nombre);
	printf("nombre:%s\n",nombreS);	
	printf("se jalo toda la data\n");

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
		printf("mensaje de error\n");
		return 0;
	}
	else{
		printf("imprimiendo usuarios %d\n",numUsers);
		agregarUsuario(originS, nombreS, socket);
		printf("se agrego usuario\n");
		return 1;
	}
}

int agregarUsuario(char *origin, char *nombre, int socket){
	printf("imprimiendo usuarios %d\n",numUsers);
	usuarios[numUsers].id = numUsers;
	usuarios[numUsers].user = nombre;
	usuarios[numUsers].status = "active";
	usuarios[numUsers].ip = origin;
	usuarios[numUsers].socket = socket;
	printf("usuario seteado\n");
	char *idS;
	sprintf(idS, "%d", numUsers);	
	printf("%s\n",idS);
	struct json_object *id = json_object_new_string(idS);
	struct json_object *name = json_object_new_string(nombre);
	struct json_object *status = json_object_new_string("activo");
	struct json_object *user = json_object_new_object();
	printf("subobjeto creado\n");
	json_object_object_add(user,"id",id);
	json_object_object_add(user,"name",name);	
	json_object_object_add(user,"status",status);
	struct json_object *reply = json_object_new_object();
	struct json_object *ok = json_object_new_string("OK");
	json_object_object_add(reply,"status",ok);
	json_object_object_add(reply,"user",user);
	printf("%s\n",json_object_get_string(reply));
	printf("%d\n",socket);
	send(socket, json_object_get_string(reply),strlen(json_object_get_string(reply)),0);
	printf("se envio la data\n");
	printf(usuarios[numUsers].user);
	printf(usuarios[numUsers].status);
	printf(usuarios[numUsers].ip);
	printf("%d\n",usuarios[numUsers].id);
	numUsers=numUsers+1;
	printf("imprimiendo usuarios %d\n",numUsers);
	return 0;
}

int cambiarEstado(json_object *jobj){
	printf("cambiando estado\n");
	json_object * user;
	json_object_object_get_ex(jobj,"user",&user);
	json_object * status;
	json_object_object_get_ex(jobj,"status",&status);
	printf("juejue\n");
	const char* userS = json_object_get_string(user);
	int userI;
	sprintf(userS,"%d",userI);
	const char* statusS = json_object_get_string(status);
	printf("user: %d\n",userI);
	int c;
	for(c=0;c<numUsers;c++){
		printf("user: %d\n",usuarios[c].id);
		if(usuarios[c].id==userI){
			printf("usuario encontrado\n");
			strcpy(usuarios[c].status,statusS);
			char * id;
			sprintf(id,"%d",usuarios[c].id);
			printf("estado cambiado a: ");
			printf(usuarios[c].status);
			notificarCambioEstado(statusS,userS,id);
			break;
		}
	}
}

int notificarCambioEstado(char * estadoS, char * userS, char * idS){
	json_object *estado;
	json_object *user;
	json_object *id;
	json_object *object = json_object_new_object();
	json_object *boletin = json_object_new_object();
	estado = json_object_new_string(estadoS);
	user = json_object_new_string(userS);
	id = json_object_new_string(idS);
	
	json_object_object_add(object,"id",id);
	json_object_object_add(object,"name",user);
	json_object_object_add(object,"status",estado);
	json_object *estadinho = json_object_new_string("CHANGED_STATUS");
	json_object_object_add(boletin,"action",estadinho);
	json_object_object_add(boletin,"user",object);
	printf(json_object_get_string(boletin));
	int c;	
	for(c=0;c<numUsers;c++){
		send(usuarios[c].socket,json_object_get_string(boletin),100,0);	
	}	
}

int listar_usuarios(json_object *jobj, int socket){
	printf("listando usuarios\n");
	json_object *reply = json_object_new_object();
	json_object *arreglo = json_object_new_array();
	char *idS;
	json_object *id;
	json_object *name;
	json_object *status;
	int c;
	for (c=0;c<9;c++){
		printf("numusers: %d\n",numUsers);
	}
	printf("numusers: %d\n",numUsers);
	for(c=0;c<numUsers;c++){
		printf(usuarios[c].user);
		//printf("iteracion %d\n",c);
		json_object *usuario = json_object_new_object();
		printf("iteracion");
		name = json_object_new_string(usuarios[c].user);
		sprintf(idS,"%d",usuarios[c].id);
		printf("%s\n",idS);
		id = json_object_new_string(idS);
		status = json_object_new_string(usuarios[c].status);
		printf("%s\n",usuarios[c].status);
		json_object_object_add(usuario,"id",id);
		json_object_object_add(usuario,"name",name);
		json_object_object_add(usuario,"status",status);				
		
		json_object_array_add(arreglo,usuario);
	}
	printf("salio\n");
	
	json_object *action = json_object_new_string("LIST_USER");
	json_object_object_add(reply,"action",action);
	json_object_object_add(reply,"users",arreglo);
	send(socket,json_object_get_string(reply),100,0);
	printf(json_object_get_string(reply));
}

int devolver_usuario(json_object *jobj, json_object *user ,int socket){
	printf("devolviendo usuario\n");
	const char *nombre = json_object_get_string(user);
	json_object *usuario = json_object_new_object();
	json_object *reply = json_object_new_object();
	json_object *arreglo = json_object_new_array();
	char *idS;
	json_object *id;
	json_object *status;
	int c;
	
	for(c=0;c<numUsers;c++){
		if(usuarios[c].user == nombre){
			status = json_object_new_string(usuarios[c].status);
			sprintf(idS,"%d",usuarios[c].id);
			json_object_object_add(usuario,"id",id);
			json_object_object_add(usuario,"name",user);
			json_object_object_add(usuario,"status",status);
			json_object_array_add(arreglo,usuario);
			json_object *action = json_object_new_string("LIST_USER");
			json_object_object_add(reply,"action",action);
			json_object_object_add(reply,"users",arreglo);
			send(socket,json_object_get_string(reply),100,0);
			printf(json_object_get_string(reply));
			break;
		}
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
	printf(toS);
	
	struct json_object *action = json_object_new_string("RECEIVE_MESSAGE");
	struct json_object *reply = json_object_new_object();
	int socket = 0;
	int c;	
	for(c=0;c<numUsers;c++){
		if(usuarios[c].id == (int)toS){
			socket = usuarios[c].socket;
			printf("%d\n",socket);
			break;
		}
	}
	json_object_object_add(reply,"action",action);
	json_object_object_add(reply,"from",from);
	json_object_object_add(reply,"to",to);
	json_object_object_add(reply,"message",message);
	send(socket,json_object_get_string(reply),100,0);
	printf(json_object_get_string(reply));
}

int sendMensajeError(char *origin, int socket){
	char *mensaje = "{\"status\": \"ERROR\", \"message\": \"Usuario ingresado ya existe.\"}";
	send(socket, mensaje, 100, 0);
	return 0;
}

