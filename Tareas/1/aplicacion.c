#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define N 1024

int abrirParametro ( char *parametroSysfs );
void leerParametro ( int fd, char *parametro, int Nbytes );
void escribirParametro ( int fd, char *parametro );

int main()
{
	int stop = 0;
	int num_elementos, i;
	int fd_opcion, fd_datos;
	char opcion[N], datos[N];

	fd_opcion = abrirParametro("/sys/module/tarea_1/parameters/opcion");
	fd_datos = abrirParametro("/sys/module/tarea_1/parameters/datos");

	while( stop == 0 )
	{
		leerParametro( fd_opcion, opcion, N );
		leerParametro( fd_datos, datos, N );

		printf( "\nQue desea hacer?\n" );
		printf( "0 - Modificar el arreglo\n" );
		printf( "1 - Obtener el promedio\n" );
		printf( "2 - Ordenar el arreglo (Burbuja)\n");
		printf( "3 - Obtener numeros pares del arreglo\n" );
		printf( "4 - Salir\n" );
		scanf( "%s", opcion );

		if( opcion[0] == '0' )
		{
			printf( "Escribe los numeros separados por comas (,):\n" );
			scanf( "%s", datos);
			escribirParametro( fd_datos, datos );
		}
		else if( opcion[0] == '4' )
		{
			stop = 1;
		}
		escribirParametro( fd_opcion, opcion );
	}

	close( fd_opcion );
	close( fd_datos );
	return 0;
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
