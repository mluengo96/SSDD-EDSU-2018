/*
   Incluya en este fichero todas las implementaciones que pueden
   necesitar compartir los módulos editor y subscriptor,
   si es que las hubiera.
*/

#include <stdio.h>
#include <string.h>
#include "edsu_comun.h"
#include "comun.h"
#include <arpa/inet.h>

Message preparar_mensaje(int evento, const char * tema, int puerto)
{
	Message msg;

	msg.op = htons(evento);
	strcpy(msg.tema.name, tema);
//printf("P prep antes msg: %d\n", puerto);
	msg.sub.port = htons(puerto);
//printf("P prep desp msg: %d\n", msg.sub.port);

	return msg;
}
