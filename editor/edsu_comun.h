/*
   Incluya en este fichero todas las definiciones que pueden
   necesitar compartir los módulos editor y subscriptor,
   si es que las hubiera.
*/

#ifndef EDSU_H
#define EDSU_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "comun.h"

Message preparar_mensaje(int evento, const char * tema, int valor);

#endif
