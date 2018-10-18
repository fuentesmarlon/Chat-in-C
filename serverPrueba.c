#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
struct sockaddr_in serv;
int fd;
int conn;
char empty[100] = "";
char message[100] ="";

int main(){
	serv.sin_family = AF_INET;
	serv.sin_port=htons(8096);
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
				strcpy(message,empty);
			}
		exit(0);
		}
	}
	
}
