#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#include "subscriptor.h"

#define NOPS 2
int (*ops[NOPS])(const char *tema) = {alta_subscripcion_tema,
		baja_subscripcion_tema};
char * ops_nom[NOPS] = {"alta subscripcion", "baja subscripcion"};

static bool leer_orden(unsigned int *op, char *tema) {
	char linea[256];
	bool leido = false;
	
	do {
        	printf("\nIntroduzca una línea con la operación (0 = alta; 1 = baja) y el tema\n");
		if (fgets(linea, 256, stdin)== NULL) return false; // EOF
		if ((sscanf(linea, "%u%64s", op, tema) == 2) && (*op<NOPS))
			leido = true;
		else
        		printf("\nError formato de entrada\n");
	} while (!leido);
	return true;
}

static void notificacion_evento(const char *t, const char *e){
        printf("\n-> Recibido evento %s con valor %s\n", t, e);
}

int main(int argc, char *argv[]) {
	unsigned int op;
	char tema[64];

	inicio_subscriptor(notificacion_evento, NULL, NULL);
	while (leer_orden(&op, tema))
		if (ops[op](tema)<0) 
                	fprintf(stderr, "Error en %s sobre tema %s\n",
				 ops_nom[op], tema);
	return 0;
}
