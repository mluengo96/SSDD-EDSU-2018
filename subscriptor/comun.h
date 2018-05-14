/*
   Incluya en este fichero todas las definiciones que pueden
   necesitar compartir todos los módulos (editor, subscriptor y
   proceso intermediario), si es que las hubiera.
*/

#ifndef COMUN_H
#define COMUN_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>

#define ALTA 	  0 /* Mensaje de alta */
#define BAJA 	  1 /* Mensaje de baja */
#define EVENTO 	  2 /* Mensaje de recepcion de evento */
#define OK	  3 /* Mensaje respuesta de OK */
#define ERROR	  4 /* Mensaje respuesta de ERROR */

typedef struct Subscriptor
{
	int id;	/* Identificador del subscriptor */
//	struct sockaddr_in myaddr;
	unsigned short port; /* Puerto del subscriptor */
//	unsigned long s_addr; /* IP del subscriptor */
} Subscriptor;

typedef struct Topic
{
	char name[10]; /* Nombre del tema */
	char valor[10];
	int subsList[256]; /* Listado de interesados en el tema */
	int contSubs; /* Numero de subscriptores inscritos en el tema */
} Topic;

typedef struct Msg_Struct
{
	int op;	/* Operacion */
	int opResp; /* Operacion de respuesta OK-ERROR */
	Subscriptor sub;
	Topic tema; /* Tema a dar de alta/baja, etc */
} Message;

#endif
