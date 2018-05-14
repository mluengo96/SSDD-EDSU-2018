#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#include "editor.h"

static bool leer_evento(char *tema, char *valor) {
        char linea[256];
        bool leido = false;

        do {
                printf("\nIntroduzca una línea con el tema y el valor del evento\n");
                if (fgets(linea, 256, stdin)== NULL) return false; // EOF
                if (sscanf(linea, "%64s%64s", tema, valor) == 2)
                        leido = true;
                else
                        printf("\nError formato de entrada\n");
        } while (!leido);
        return true;
}

int main(int argc, char *argv[]) {
	char tema[64];
	char valor[64];

        while (leer_evento(tema, valor))
		if (generar_evento(tema, valor)<0)
			fprintf(stderr, "Error generando evento para tema %s\n",
                                 tema);
	return 0;
}
