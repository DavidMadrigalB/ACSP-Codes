/*
 *  aplicacion.c Aplicación de usuario sobre la tarea 4,
 *  arreglo de enteros con distintas funciones haciendo uso de funciones call back
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX 1024

int abrirParametro ( char *parametroSysfs );
void leerParametro ( int fd, char *parametro, int Nbytes );
void escribirParametro ( int fd, char *parametro );

int main()
{
	/* VARIABLES */
	char datos[MAX];
	char opcion[MAX] = "0";
	/* DESCRIPTORES */
	int fd_datos;
	int fd_opcion;

	fd_opcion = abrirParametro("/sys/module/tarea_4/parameters/opcion");
	fd_datos = abrirParametro("/sys/module/tarea_4/parameters/datos");

	printf("Obteniendo los datos... \n");
	printf("Boleta: ");
	leerParametro(fd_datos, datos, MAX);
	printf("\n");

	do
	{
		/* MENU */
		printf("Opciones: \n");
		//printf(" 0 - Ingresar datos (arreglo de enteros) \n");
		printf(" 1 - Calcular el promedio del arreglo \n");
		printf(" 2 - Ordenar el arreglo (usando burbuja) \n");
		printf(" 3 - Obtener los números pares del arreglo \n");
		printf(" 4 - Salir \n");
		printf("Ingresa una opción: ");

		scanf("%s", opcion);

		switch(opcion[0])
		{
			/*
			case '0':
				printf("Ingresa los números separados por comas (,): \n");
				printf("(Ejemplo : 1,2,3) \n");
				printf("Números: ");
				scanf("%s", datos);
				escribirParametro( fd_datos, datos );
				break;
			*/
			case '1':
			case '2':
			case '3':
				escribirParametro( fd_opcion, opcion );
				break;
			case '4':
				printf("Gracias por usar el programa, hasta luego. \n");
				break;
			default:
				printf("Por favor, eliga una opción válida. \n");
				break;
		}
	}while(opcion[0] != '4');
}

int abrirParametro( char *parametroSysfs )
{
	int fd;
	fd = open( parametroSysfs, O_RDWR );
	if( fd == -1 )
	{
		perror( "Error al abrir el parametro\n" );
		exit( EXIT_FAILURE );
	}
	printf( "El descriptor del parametro es: %d\n", fd );
	return fd;
}

void leerParametro( int fd, char *parametro, int Nbytes )
{
	int nbytes;
	nbytes = read( fd, parametro, Nbytes );
	if( nbytes == -1 )
	{
		perror( "Error al leer el parametro\n" );
		exit( EXIT_FAILURE );
	}

	parametro[ nbytes - 1 ] = 0;
	printf( "Bytes recibidos del parametro: %d\n", nbytes );
	printf( "El valor del parametro es: %s\n", parametro );
}

void escribirParametro( int fd, char *parametro )
{
	int nbytes;
	nbytes = write( fd, parametro, strlen(parametro) + 1 );
	if( nbytes == -1 )
	{
		perror( "Error al escribir en el parametro\n" );
		exit( EXIT_FAILURE );
	}
	printf( "Bytes enviados al parametro: %d\n", nbytes );
}
