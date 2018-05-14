#include "editor.h"
#include "comun.h"
#include "edsu_comun.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

Message preparar_mensaje2(int evento, const char * tema, const char * valor)
{
	Message msg;
	msg.op = htons(evento);
	strcpy(msg.tema.name, tema);
	strcpy(msg.tema.valor, valor);
printf("EDITOR: Valor del tema: %s\n", msg.tema.valor);

return msg;
}

int generar_evento(const char *tema, const char *valor) {
	int es_sd;
	struct sockaddr_in essa_in;
	char * servidor;
	char * puerto;
	Message msg;
	int errnum;

	/* Creacion del socket TCP para generar eventos */
	es_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if(es_sd < 0)
	{
		errnum = errno;
                fprintf(stderr, "Valor de errno: %d\n", errno);
                perror("Error devuelto por perror");
		fprintf(stderr, "EDITOR: Creacion del socket TCP: ERROR\n");
                return -1;
	}

	msg = preparar_mensaje2(2, tema, valor);

	/* Nombre de la maquina donde ejecuta el intermediario */
	servidor = getenv("SERVIDOR");
	
	if(servidor == NULL)
	{
		fprintf(stderr, "EDITOR: SERVIDOR no se encuentra definida\n");
	}

	/* Numero de puerto TCP por el que esta escuchando */
	puerto = getenv("PUERTO");

	if(puerto == NULL)
        {
                fprintf(stderr, "EDITOR: PUERTO no se encuentra definida\n");
        }
	
	/* Ponemos a 0 la estructura */
	bzero((char*)&essa_in, sizeof(struct sockaddr_in));
	
	essa_in.sin_family = AF_INET;
//	essa_in.sin_addr.s_addr = inet_addr(servidor);
	essa_in.sin_port = htons(atoi(puerto));

	/* Conecta con el servidor */
	if(connect(es_sd, (struct sockaddr*)&essa_in, sizeof(struct sockaddr_in)) < 0)
	{
		errnum = errno;
                fprintf(stderr, "Valor de errno: %d\n", errno);
                perror("Error devuelto por perror");
		fprintf(stderr, "EDITOR: Error al conectar con el servidor");
		close(es_sd);
		return -1;
	}

	/* Enviamos el evento */
	if(send(es_sd, &msg, sizeof(msg), 0) < 0)
        {
                fprintf(stderr, "EDITOR: Error al enviar el evento\n");
                return -1;
        }

        fprintf(stdout, "EDITOR: Evento enviado a intermediario\n");

        /* Recibimos respuesta */
        if(recv(es_sd, &msg, sizeof(msg), 0) < 0)
        {
                fprintf(stderr, "EDITOR: Error al recibir respuesta del envio del evento\n");
                return -1;
        }

        fprintf(stdout, "EDITOR: Respuesta recibida correctamente\n");

printf("Mensaje respuesta: %d\n", ntohs(msg.op));

        /* Cerramos la conexion */
        close(es_sd);

	return 0;
}

/* solo para la version avanzada */
int crear_tema(const char *tema) {
	return 0;
}

/* solo para la version avanzada */
int eliminar_tema(const char *tema) {
	return 0;
}

