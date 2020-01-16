# Práctica uso de fork
Práctica sobre uso básico de fork en Linux usando C para la materia Programación de Sistemas (CCPG1008) de la ESPOL.

## Instrucciones
La práctica consiste en crear una aplicación cliente - servidor para ejecutar un comando de manera remota. La aplicación debe implementar la ejecución remota de comandos en el servidor de la siguiente forma:
1. El cliente debe preguntar al usuario el comando que desea ejecutar desde un prompt en consola.
2. El cliente debe enviar esta información al servidor a través de un socket TCP/IP.
3. El servidor recibe el comando y lo reconstruye para ser ejecutado usando [execvp](https://www.gnu.org/software/libc/manual/html_node/Executing-a-File.html).
4. El servidor debe responder "ERROR\n" si el archivo no se pudo ejecutar u "OK\n" si se ejecutó exitosamente.
5. El servidor debe esperar el envío de un nuevo comando.

Toda comunicación entre cliente y servidor debe ser enviada en modo ASCII con salto de línea al final.

El programa servidor *server* tiene el siguiente comportamiento:
```
$ ./server -h
Servidor simple de ejecución remota de comandos.
uso:
 ./server <puerto>
 ./server -h
Opciones:
 -h			Ayuda, muestra este mensaje
```

El programa cliente *client* tiene el siguiente comportamiento:
```
$ ./client -h
Cliente simple de ejecución remota de comandos.
uso:
 ./client <hostname> <puerto>
 ./client -h
Opciones:
 -h			Ayuda, muestra este mensaje
```
Asumiendo que el servidor esta corriendo en una maquina con la IP 192.168.100 en el puerto 8080. Ejemplo de ejecución del cliente:
```
$ ./client 192.168.100 8080
Conectado exitosamente a 192.168.100 en el puerto 8080.
> ls -lh
OK
> sadfsad
ERROR
```
La salida del comando ejecutado remotamente en el servidor debe ser mostrada en la consola del servidor.

## Código ejemplo
Este repositorio contiene código ejemplo de una aplicación *eco* cliente - servidor. Es decir, el usuario ingresa una línea de texto desde el cliente, el cliente la envia al servidor y el servidor la envia de regreso.

En [server.c](server.c) se provee un ejemplo de separación de una cadena de caracteres en tokens usando la función *strtok()*:

```C
/**
 * Función ejemplo para separar una cadena de caracteres en
 * "tokens" delimitados por la cadena de caracteres delim.
 *
 * @param linea Cadena de caracteres a separar en tokens.
 * @param delim Cadena de caracteres a usar como delimitador.
 */
void separar_tokens(char *linea, char *delim)
{
	char *token;
	int i = 0;

	printf("Sepración en tokens:\n");

	/* obtiene el primer token */
	token = strtok(linea, delim);

	/* recorre todos los tokens */
	while( token != NULL ) {
		printf( "%d. %s\n", ++i, token);
		token = strtok(NULL, delim);
	}
}
```

## Entregable
Modificar el código ejemplo en este repositorio para implementar la aplicación cliente - servidor de ejecución remota de comando. El servidor debe usar *fork()* para atender varios clientes de manera simultanea y *fork()* + *execvp()* para ejecutar el comando enviado usando un proceso hijo. 

## Compilación
Para compilar cliente y servidor:
```
$ make
```
Para compilar solo el servidor:
```
$ make server
```
Para compilar cliente y servidor facilitando la depuración con gdb:
```
$ make debug
```
Para compilar cliente y servidor habilitando la herramienta AddressSanitizer, facilita la depuración en tiempo de ejecución:
```
$ make sanitize
```
## Integrantes
Kelly Vaque: concurrencia con fork
Katiuska Marín: implementacion de funcion execvp
