#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "subscriptor.h"
#include "comun.h"
#include "edsu_comun.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

unsigned short subPort;
char * subPortC;
Message msg, msgR;
pthread_mutex_t lock;

static void (*notif)(const char *, const char *);

void * thread(int sd){
//	Message msg;
	int th_sd;
	struct sockaddr_in sa_in;
	socklen_t sa_in_size = sizeof(struct sockaddr_in);
	int errnum;

	bzero((char*)&sa_in, sizeof(struct sockaddr_in));

	while(1)
	{
		if((th_sd = accept(sd, (struct sockaddr*)&sa_in, &sa_in_size)) < 0)	
		{
			errnum = errno;
			fprintf(stderr, "Valor de errno: %d\n", errno);
			perror("Error devuelto por perror");
			fprintf(stderr, "SUBSCRIPTOR: Error al conectar con el puerto P: %s\n", strerror(errnum));
			close(sd);
			pthread_exit(NULL);
			exit(1);
		}

		fprintf(stdout, "SUBSCRIPTOR: Conexion a P realizada con exito\n");

		if(recv(th_sd, &msg, sizeof(msg), 0) < 0)
		{
			fprintf(stderr, "SUBSCRIPTOR: Error al recibir mensaje del intermediario\n");
			close(sd);
			close(th_sd);
			exit(1);
		}

		fprintf(stdout, "SUBSCRIPTOR: Recibido mensaje de intermediario\n");

		(*notif)(msg.tema.name, msg.tema.valor);

		//printf("HOLA\n");
		close(th_sd);
		//printf("HOLA2\n");
//		break;
	}
		//printf("HOLA3\n");
	close(sd);
		//printf("HOLA4\n");
	pthread_exit(NULL);
		//printf("HOLA5\n");
}

int alta_subscripcion_tema(const char *tema) {
	int at_sd;
	struct sockaddr_in atsa_in;
//	socklen_t ats_size;
	char * servidor;
	char * puerto;
//	Message msg, msgR;
	int errnum;

	/* Creacion del socket */
	at_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(at_sd < 0)
	{
		errnum = errno;
		fprintf(stderr, "Valor de errno: %d\n", errno);
		perror("Error devuelto por perror");
		fprintf(stderr, "SUBSCRIPTOR: Creacion del socket TCP de alta: %s\n", strerror(errnum));
                return -1;
	}
printf("P en ALTA: %d\n", ntohs(subPort));
//printf("Tema en ALTA: %s\n", tema);

	/* Preparamos el mensaje de ALTA a un tema */
	msg = preparar_mensaje(0, tema, subPort);
	
	/* Nombre de la maquina donde ejecuta el intermediario */
        servidor = getenv("SERVIDOR");

        if(servidor == NULL)
        {
                fprintf(stderr, "SUBSCRIPTOR: Variable SERVIDOR no se encuentra definida\n");
        }

        /* Numero de puerto TCP por el que esta escuchando */
        puerto = getenv("PUERTO");

        if(puerto == NULL)
        {
                fprintf(stderr, "SUBSCRIPTOR: Variable PUERTO no se encuentra definida\n");
        }

	/* Ponemos a 0 la estructura */
	bzero((char*)&atsa_in, sizeof(struct sockaddr_in));

	atsa_in.sin_family = AF_INET;
//	atsa_in.sin_addr.s_addr = inet_addr(servidor); // PROVOCA NETWORK IS UNREACHABLE
	atsa_in.sin_port = htons(atoi(puerto));

	/* Conectamos con el servidor */
	if(connect(at_sd, (struct sockaddr*)&atsa_in, sizeof(struct sockaddr_in)) < 0)
	{
		errnum = errno;
                fprintf(stderr, "Valor de errno: %d\n", errno);
                perror("Error devuelto por perror");
		fprintf(stderr, "SUBSCRIPTOR: Error al conectar con intermediario: %s\n", strerror(errnum));
		close(at_sd);
		return -1;
	}
/////////////////////
// LINEA DE PRUEBA //
/////////////////////
//	msg.op = htons(ALTA);
//	printf("%d\n", msg.puerto);	
printf("Tema a enviar: %s\n", msg.tema.name);	
	/* Enviamos */
	if(send(at_sd, &msg, sizeof(msg), 0) < 0)
	{
		fprintf(stderr, "SUBSCRIPTOR: Error al enviar la peticion de alta\n");
		return -1;
	}

	fprintf(stdout, "SUBSCRIPTOR: Peticion de alta enviada a intermediario\n");

	/* Recibimos respuesta */
	if(recv(at_sd, &msgR, sizeof(msgR), 0) < 0)
	{
		fprintf(stderr, "SUBSCRIPTOR: Error al recibir respuesta de peticion de alta\n");
		return -1;
	}

	fprintf(stdout, "SUBSCRIPTOR: Respuesta recibida correctamente\n");

printf("Mensaje respuesta: %d\n", ntohs(msgR.opResp));

	/* Cerramos la conexion */
	close(at_sd);

	return 0;
}

int baja_subscripcion_tema(const char *tema) {
	int bt_sd;
        struct sockaddr_in btsa_in;
	char * servidor;
	char * puerto;
//	Message msg, msgR;
	int errnum;

	bt_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(bt_sd < 0)
	{
		errnum = errno;
		fprintf(stderr, "Valor de errno: %d\n", errno);
		perror("Error devuelto por perror");
		fprintf(stderr, "SUBSCRIPTOR: Creacion del socket TCP de baja: %s\n", strerror(errnum));
                return -1;
	}

	printf("P en BAJA: %d\n", subPort);
//	printf("Tema en BAJA: %s\n", tema);

	/* Preparamos el mensaje de ALTA a un tema */
	msg = preparar_mensaje(1, tema, subPort);

	/* Nombre de la maquina donde ejecuta el intermediario */
	servidor = getenv("SERVIDOR");
	if(servidor == NULL)
	{
		fprintf(stderr, "SUBSCRIPTOR: Variable SERVIDOR no se encuentra definida\n");
	}

	/* Numero de puerto TCP por el que esta escuchando */
	puerto = getenv("PUERTO");
	if(puerto == NULL)
	{
		fprintf(stderr, "SUBSCRIPTOR: Variable PUERTO no se encuentra definida\n");
	}

	/* Ponemos a 0 la estructura */
	bzero((char*)&btsa_in, sizeof(struct sockaddr_in));

	btsa_in.sin_family = AF_INET;
//	btsa_in.sin_addr.s_addr = inet_addr(servidor); // PROVOCA NETWORK IS UNREACHABLE
	btsa_in.sin_port = htons(atoi(puerto));

	/* Conectamos con el servidor */
	if(connect(bt_sd, (struct sockaddr*)&btsa_in, sizeof(struct sockaddr_in)) < 0)
	{
		errnum = errno;
		fprintf(stderr, "Valor de errno: %d\n", errno);
		perror("Error devuelto por perror");
		fprintf(stderr, "SUBSCRIPTOR: Error al conectar con intermediario: %s\n", strerror(errnum));
		close(bt_sd);
		return -1;
	}

	/* Enviamos */
	if(send(bt_sd, &msg, sizeof(msg), 0) < 0)
	{
		fprintf(stderr, "SUBSCRIPTOR: Error al enviar la peticion de baja\n");
		return -1;
	}

	fprintf(stdout, "SUBSCRIPTOR: Peticion de baja enviada a intermediario\n");

	/* Recibimos respuesta */
	if(recv(bt_sd, &msgR, sizeof(msgR), 0) < 0)
	{
		fprintf(stderr, "SUBSCRIPTOR: Error al recibir respuesta de peticion de baja\n");
		return -1;
	}

	fprintf(stdout, "SUBSCRIPTOR: Respuesta recibida correctamente");

	/* Cerramos la conexion */
	close(bt_sd);

	return 0;
}

int inicio_subscriptor(void (*notif_evento)(const char *, const char *),
                void (*alta_tema)(const char *),
                void (*baja_tema)(const char *)) {
	int sub_sd;
	struct sockaddr_in sub_sa_in;
	socklen_t sub_s_size;
	pthread_t th1;
	pthread_attr_t attr;

	notif = notif_evento;

	/* Creacion del socket TCP para recibir eventos */
	sub_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(sub_sd < 0)
	{
		fprintf(stderr, "SUBSCRIPTOR: Creacion del socket TCP de inicio: ERROR\n");
		return -1;
	}

	/* Hay que dejar la estructura a 0 antes de usarla */
	bzero((char*)&sub_sa_in, sizeof(struct sockaddr_in));

	sub_sa_in.sin_family = AF_INET; // Dominio basado en direcciones IPv4
	sub_sa_in.sin_port = htons(0); // Asigna el puerto que haya libre
	sub_s_size = sizeof(sub_sa_in);

//printf("P en INICIO: %d\n", sub_sa_in.sin_port);

	/* Asignacion de la direccion local Puerto TCP P libre */
	if(bind(sub_sd, (struct sockaddr*)&sub_sa_in, sub_s_size) < 0)
	{
		fprintf(stderr, "SUBSCRIPTOR: Asignacion del puerto: ERROR\n");
		close(sub_sd);
		return -1;
	}
	getsockname(sub_sd, (struct sockaddr*)&sub_sa_in, &sub_s_size);
	subPort = sub_sa_in.sin_port;

//	subPortC = (char)subPort;
//printf("subPort = %d\n", subPort);
//printf("subPortC = %d\n", subPortC);

	/* Aceptamos conexiones por el socket TCP */
	if(listen(sub_sd, 5) < 0)
	{
		fprintf(stderr, "SUBSCRIPTOR: Aceptacion de peticiones: ERROR\n");
		close(sub_sd);
		return -1;
	}

	fprintf(stdout, "SUBSCRIPTOR: Esperando confirmacion del intermediario\n");

	struct sockaddr_in aux;
	int sz = sizeof(aux);
	getsockname(sub_sd, (void*)&aux, (socklen_t *)&sz);
	
	/* Iniciamos los atributos y los marcamos como independientes */
//	pthread_attr_init(&attr);
//	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	/* Creamos el thread con los atributos */
	pthread_create(&th1, NULL, thread, (void*)sub_sd);

	return 0;
}

int fin_subscriptor() {
	return 0;
}

