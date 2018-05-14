#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "editor.h"

#define NOPS 3
int (*ops[NOPS])(const char *tema, const char *valor) =
	{generar_evento, (int (*)(const char *, const char *))crear_tema,
	(int (*)(const char *, const char *))eliminar_tema};

char * ops_nom[NOPS] = {"generar evento", "crear tema", "eliminar tema"};

static bool leer_orden(unsigned int *op, char *tema, char *valor) {
        char linea[256];
	int n;
        bool leido = false;

        do {
                printf("\nIntroduzca una línea con la operación (0 = generar; 1 = crear tema; 2 = eliminar tema), el tema y el valor (solo para generar)\n");
                if (fgets(linea, 256, stdin)== NULL) return false; // EOF
		n = sscanf(linea, "%u%64s%64s", op, tema, valor);
                if (((n >= 2) && (*op<NOPS)) &&
				!((*op == 0) && (n<3)))
                        leido = true;
               else
                        printf("\nError formato de entrada\n");
        } while (!leido);
        return true;
}

int main(int argc, char *argv[]) {
        unsigned int op;
	char tema[64];
	char valor[64];

        while (leer_orden(&op, tema, valor))
                if (ops[op](tema, valor)<0)
                        fprintf(stderr, "Error en %s sobre tema %s\n",
                                 ops_nom[op], tema);

        return 0;
}
