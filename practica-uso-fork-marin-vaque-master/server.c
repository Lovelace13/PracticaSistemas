#include <getopt.h>
#include <limits.h>

#include "common.h"

void atender_cliente(int connfd);
void sigchldhandler(int sig);

void print_help(char *command)
{
	printf("Servidor simple de ejecucion remota de comandos\n");
	printf("uso:\n %s <puerto>\n", command);
	printf(" %s -h\n", command);
	printf("Opciones:\n");
	printf(" -h\t\t\tAyuda, muestra este mensaje\n");
}

/**
 * Función ejemplo para separar una cadena de caracteres en
 * "tokens" delimitados por la cadena de caracteres delim.
 *
 */
void separar_tokens(char *linea, char **argumentos)
{
	char *delim;
	int i = 0;
	
	linea[strlen(linea)-1]=' ';
	while (*linea && (*linea == ' ')) /* Ignore leading spaces */
	{ 													
		linea++;
	}

	while((delim=strchr(linea,' ')))
	{
		argumentos[i++] = linea;
		*delim = '\0';
		linea = delim + 1;
		while(*linea && (*linea==' '))
		{
			linea++;
		}
	}
	argumentos[i]=NULL;
}

int main(int argc, char **argv)
{
	int opt;

	//Sockets
	int listenfd, connfd;
	unsigned int clientlen;
	//Direcciones y puertos
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	char *haddrp, *port;
	pid_t pid;

	while ((opt = getopt (argc, argv, "h")) != -1){
		switch(opt)
		{
			case 'h':
				print_help(argv[0]);
				return 0;
			default:
				fprintf(stderr, "uso: %s <puerto>\n", argv[0]);
				fprintf(stderr, "     %s -h\n", argv[0]);
				return -1;
		}
	}

	if(argc != 2){
		fprintf(stderr, "uso: %s <puerto>\n", argv[0]);
		fprintf(stderr, "     %s -h\n", argv[0]);
		return -1;
	}else
		port = argv[1];

	//Valida el puerto
	int port_n = atoi(port);
	if(port_n <= 0 || port_n > USHRT_MAX){
		fprintf(stderr, "Puerto: %s invalido. Ingrese un número entre 1 y %d.\n", port, USHRT_MAX);
		return -1;
	}

	//Abre un socket de escucha en port
	listenfd = open_listenfd(port);

	if(listenfd < 0)
		connection_error(listenfd);

	printf("server escuchando en puerto %s...\n", port);

	while (1) {
		clientlen = sizeof(clientaddr);
		connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen); //descriptor de conexion

		/* Determine the domain name and IP address of the client */
		hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);

		printf("server conectado a %s (%s)\n", hp->h_name, haddrp);
		signal(SIGCHLD,sigchldhandler);
		if((pid=fork())==0){
			close(listenfd);
			
			
			atender_cliente(connfd);

			printf("server desconectando a %s (%s)\n", hp->h_name, haddrp);
			close(connfd);
			exit(0);

		}
		
		
		
	}
	close(connfd);
}

void atender_cliente(int connfd)
{
	int n, status;
	char buf[MAXLINE] = {0};

	while(1){
		n = read(connfd, buf, MAXLINE); //lee lo que le envia el cliente
		if(n <= 0)
			return;

		//Ejecucion de de comandos segun el write
		printf("Recibido: %s", buf);

		char *argumentos[4];
		int i = 0;
		int *puntero;
		puntero = &i;
		pid_t pid2;


		separar_tokens(buf,argumentos);

		char buf[100]="/bin/";
		strcpy(buf,argumentos[0]);
		
		//signal(SIGCHLD,sigchldhandler);
		pid2 = fork();
		if( pid2 == 0 ){ //hijo
			i = execvp(argumentos[0], argumentos);
			if( *puntero < 0){
				printf("Commando del cliente no valido: %s\n", buf);
				write(connfd,"ERROR\n",6);
				exit(100);
			}		
		}	

		while((pid2 =(waitpid(-1, &status, 0))) > 0)
		{
			if(WIFEXITED(status) && !(WEXITSTATUS(status))){
				write(connfd,"OK...\n",6);
				printf("Comando ejecutado. Proceso %d terminado\n", pid2);
			}
			else if(WIFEXITED(status) && (WEXITSTATUS(status)))
				printf("Comando no ejecutado. Proceso %d terminado con Exit status= %d\n", pid2, WEXITSTATUS(status));
			else
				printf("hijo %d terminado anormalmente\n", pid2);
		}

		//Detecta "CHAO" y se desconecta del cliente
		if(strcmp(buf, "CHAO\n") == 0){
			write(connfd, "BYE\n", 4);
			return;
		}

		//Remueve el salto de linea antes de extraer los tokens
		buf[n - 1] = '\0';

		

		memset(buf, 0, MAXLINE); //Encera el buffer
	}
}

void sigchldhandler(int sig){
	while(waitpid(-1,0,WNOHANG)>0)
	;
	return;
}

