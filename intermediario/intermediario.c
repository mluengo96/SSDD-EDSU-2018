#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "comun.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>

int  Numero_Temas(FILE * file);

size_t cont = 0; /* Contador de subscriptores dados de alta */

int Numero_Temas(FILE *file)
{
	int rLines = 0;
        int rChar;

	while(!feof(file))
        {
                rChar = fgetc(file);
                if(rChar == '\n')
                {
                        rLines++;
                }
        }
	
	fseek(file, 0, SEEK_SET);
	return rLines;
}

int main(int argc, char *argv[])
{
	int sd, cd;
	struct sockaddr_in sa_in, ca_in;
	socklen_t ssize, csize;
	FILE * file;
	Message msg;
	int fSize;
	int errnum;
	int listaSubs[256];
	char nameTopic[10]; /* Copia del nombre del tema */
//	Topic* tAux = malloc(sizeof(Topic*));

	if (argc != 3)
	{
		fprintf(stderr, "Uso: %s puerto fichero_temas\n", argv[0]);
		return -1;
	}

	/* Estructura del cliente a 0 */
	bzero((char*)&ca_in, sizeof(struct sockaddr_in));
	csize = sizeof(ca_in);
	
	/* Contador de numero de filas del fichero para determinar numero de temas */
	file = fopen(argv[2], "r");

	if (file == NULL)
	{
		fprintf(stderr, "INTERMEDIARIO: Error al abrir el fichero %s\n", argv[2]);
		return -1;
	}

	fSize = Numero_Temas(file);
	int ct = 0;
	struct Topic listaTemas[fSize];
	
	while (ct < fSize)
	{
		if (fgets(listaTemas[ct].name, sizeof(listaTemas[ct].name), file) != NULL)
		{
//			puts(listaTemas[ct].name);
//			printf("%s\n", listaTemas[ct].name);
			size_t ln = strlen(listaTemas[ct].name)-1;
			if(listaTemas[ct].name[ln] == '\n')
			{
				listaTemas[ct].name[ln] = '\0';
			}
			ct++;
		}
		else
		{
			printf("ERROR fgets\n");
			fclose(file);
			return -1;
		}
	}

	/* Creacion del socket TCP */
	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if(sd < 0)
	{
		fprintf(stderr, "INTERMEDIARIO: Creacion del socket TCP: ERROR\n");
		return -1;
	}
	
	/* Hay que dejar la estructura a 0 antes de usarla */
	bzero((char*)&sa_in, sizeof(struct sockaddr_in));

	sa_in.sin_family = AF_INET; // Dominio basado en direcciones IPv4
	sa_in.sin_port = htons(atoi(argv[1])); // Asigna el puerto pasado como argumento
	ssize = sizeof(sa_in);

	/* Asignacion de la direccion local Puerto TCP */
	if(bind(sd, (struct sockaddr*)&sa_in, ssize) < 0)
	{
		fprintf(stderr, "INTERMEDIARIO: Asignacion del puerto: ERROR\n");
		close(sd);
		return -1;
	}

	/* Aceptamos conexiones por el socket */
	if(listen(sd, 5) < 0)
	{
		fprintf(stderr, "INTERMEDIARIO: Aceptacion de peticiones: ERROR\n");
		close(sd);
		return -1;
	}

	while(1) /* Bucle para procesar las peticiones */
	{
		//bzero((char*)&msg, sizeof(Message));

		fprintf(stdout, "INTERMEDIARIO: Esperando peticion\n");

		/* Se obtiene la IP del cliente (Subscriptor) */
		cd = accept(sd, (struct sockaddr*)&ca_in, &csize);
	
		if(cd < 0)
		{
			fprintf(stderr, "INTERMEDIARIO: Error al obtener la IP del cliente\n");
			return -1;
		}

		fprintf(stdout, "INTERMEDIARIO: IP del cliente recibida\n");

		/* Recibe el mensaje del subscriptor y lo almacena en msg */
		if(recv(cd, &msg, sizeof(msg), 0) < 0)
		{
			errnum = errno;
			fprintf(stderr, "Valor de errno: %d\n", errno);
			perror("Error devuelto por perror");
			fprintf(stderr, "INTERMEDIARIO: Error al recibir: %s\n", strerror(errnum));
			return -1;
		}

		fprintf(stdout, "INTERMEDIARIO: Mensaje del subscriptor recibido\n");

		int tam = sizeof(listaTemas)/sizeof(listaTemas[0]);

		/* Comprobamos que el tema esta en la lista */
		int l;
		int k = 0;

		for(l = 0; l < tam; l++)
		{
			if(strcmp(msg.tema.name, listaTemas[l].name) == 0)
				break;
			else
				k++;
		}

		if(k == tam)
		{
			printf("INTERMEDIARIO: El tema solicitado %s no existe\n", msg.tema.name);
		}
		else
		{
			printf("OP Int: %d\n", ntohs(msg.op));
			printf("Tema: %s\n", msg.tema.name);
			printf("P IntS: %d\n", msg.sub.port);

			memcpy(msg.tema.subsList, listaSubs, 256*sizeof(int));

			size_t size_subsList = sizeof(msg.tema.subsList) / sizeof(msg.tema.subsList[0]); /* Longitud de la lista de subscriptores */

			if(msg.op == ntohs(ALTA)) /* Mensaje ALTA */
			{
				// INTRODUCIR SUBSCRIPTOR A LA LISTA DEL TEMA
				// MENSAJE DE ESTADO
				printf("INTERMEDIARIO: ALTA solicitada\n");
		
//				tAux = &msg.tema;
//				cont = msg.tema.contSubs;

				if(cont < size_subsList)
				{
//					msg.sub.id = (int) (msg.tema.contSubs + 1);
					int i = 0;

					/* Un sub se puede dar de alta de distintos temas */
					if((cont >= 1) && (strcmp(msg.tema.name, nameTopic) == 0))
					{
						while(i < cont) /* Comprobamos entre la lista de subscriptores dados de alta */
						{
							/* Comprobamos que el subscriptor no este dado de ALTA */
							if(msg.sub.port != listaSubs[i])
							{
								i++;
							}
							else
							{
								/* Ha ocurrido un error al dar de alta al subscriptor */
								msg.opResp = htons(ERROR); /* Mensaje de respuesta */
								printf("INTERMEDIARIO: ERROR al dar de ALTA al subscriptor %d\n", msg.sub.port);
								break;
							}
						}
					}
	//				cont = 0;
					if(msg.opResp != ntohs(ERROR))
					{
	//					msg.sub.id = (int) (cont+1);
	//					msg.tema.subsList[cont] = msg.sub.port;
						listaSubs[cont] = msg.sub.port;
		printf("Nombre tema: %s\n", msg.tema.name);
						strcpy(nameTopic, msg.tema.name);
		printf("Copia nombre tema: %s\n", nameTopic);
						msg.opResp = htons(OK); /* Mensaje de respuesta */
						printf("INTERMEDIARIO: ALTA del subscriptor %d realizada correctamente\n", msg.sub.port);
						msg.tema.subsList[cont] = listaSubs[cont];
//						msg.tema.contSubs++;
						cont++;
						printf("INTERMEDIARIO: Numero de subscriptores %zu en el tema %s\n", cont, msg.tema.name);
					}
				}

				else
				{
					errnum = errno;
					fprintf(stderr, "Valor de errno: %d\n", errno);
					perror("Error devuelto por perror");
					fprintf(stderr, "INTERMEDIARIO: Limite de inscripciones alcanzado %s\n", strerror(errnum));
				}
			}
		
			else if(msg.op == ntohs(BAJA)) /* Mensaje BAJA */
			{
				// ELIMINAR SUSBCRIPTOR DE LA LISTA DE INTERESADOS
				// MENSAJE DE ESTADO
				printf("INTERMEDIARIO: BAJA solicitada\n");
printf("Subs: %zu\n", cont);
				int i = 0;
				while(i < cont)
				{
					/* Si subscriptor esta en la lista, se elimina */
//					if(msg.tema.subsList[i] == msg.sub.port)
					if(listaSubs[i] == msg.sub.port)
					{
						int c = i;
//						int aux = i;
						while(c < cont)
						{
//							msg.tema.subsList[c] = msg.tema.subsList[c+1];
							listaSubs[c] = listaSubs[c+1];
							msg.tema.subsList[c] = listaSubs[c];
							c++;
						}
						msg.opResp = htons(OK); /* Mensaje de respuesta */
						cont--;
					
						printf("INTERMEDIARIO: BAJA del subscriptor %d realizada correctamente\n", msg.sub.port);

	//					break;
					}
					else
						i++;
				}
				
				if(msg.opResp != ntohs(OK))
				{
					msg.opResp = htons(ERROR); /* Mensaje de respuesta */
				}

			}

			else if(msg.op == ntohs(EVENTO))/* Mensaje EVENTO */
			{
				int sd_sub;
				struct sockaddr_in sub_addr;
				socklen_t sub_size;

				int i = 0;
				while(i <= msg.tema.contSubs)
				{
				//	msg.tema.subsList[i]
					sd_sub = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

					if(sd_sub < 0)
					{
						fprintf(stderr, "INTERMEDIARIO: Creacion del socket TCP de EVENTO: ERROR\n");
						return -1;
					}
	
					/* Hay que dejar la estructura a 0 antes de usarla */
					bzero((char*)&sub_addr, sizeof(struct sockaddr_in));

					sub_addr.sin_family = AF_INET; // Dominio basado en direcciones IPv4
					sub_addr.sin_port = htons(msg.tema.subsList[i]); // Asigna el puerto de cada subscriptor
					sub_size = sizeof(sub_addr);

					/* Conectamos con el servidor */
					if(connect(sd_sub, (struct sockaddr*)&sub_addr, sizeof(struct sockaddr_in)) < 0)
					{
						errnum = errno;
						fprintf(stderr, "Valor de errno: %d\n", errno);
						perror("Error devuelto por perror");
						fprintf(stderr, "INTERMEDIARIO: Error al conectar con subscriptor: %s\n", strerror(errnum));
						close(sd_sub);
						return -1;
					}

					/* Enviamos */
					if(send(sd_sub, &msg, sizeof(msg), 0) < 0)
					{
						fprintf(stderr, "INTERMEDIARIO: Error al enviar evento\n");
						return -1;
					}
	
					fprintf(stdout, "INTERMEDIARIO: Evento enviado correctamente\n");
	
					close(sd_sub);
					i++;

				}
			}

			else
			{
				errnum = errno;
				fprintf(stderr, "Valor de errno: %d\n", errno);
				perror("Error devuelto por perror");
				fprintf(stderr, "INTERMEDIARIO: Error al recibir %s\n", strerror(errnum));
				return -1;
			}

		}
		printf("Mensaje de respuesta: %d\n", ntohs(msg.opResp));
		if(send(cd, &msg, sizeof(msg), 0) < 0)
		{
			fprintf(stderr, "INTERMEDIARIO: Error al enviar la confirmacion\n");
                	return -1;
		}

		fprintf(stdout, "INTERMEDIARIO: Mensaje de respuesta enviado correctamente\n");

int q = 0;
for(q = 0; q < 5; q++)
{
	printf("Listado del tema %s: %d\n", msg.tema.name, msg.tema.subsList[q]);
}
	}
	return 0;
}
