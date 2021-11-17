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
	int fd_opcion, fd_cadena, fd_letra;
	char opcion[N], cadena[N], letra[N];

	fd_opcion = abrirParametro("/sys/module/tarea_2/parameters/opcion");
	fd_cadena = abrirParametro("/sys/module/tarea_2/parameters/cadena");
	fd_letra= abrirParametro("/sys/module/tarea_2/parameters/letra");

	while( stop == 0 )
	{
		leerParametro( fd_opcion, opcion, N );
		leerParametro( fd_cadena, cadena, N );

		printf( "\nQue desea hacer?\n" );
		printf( "0 - Modificar la cadena\n" );
		printf( "1 - Contar algún caracter. Ingresa un carácter.\n" );
		printf( "2 - Determina la longitud de la cadena\n");
		printf( "3 - Salir\n" );
		scanf( " %[^\n]", opcion );

		if( opcion[0] == '0' )
		{
			//printf( "Escribe el numero de elementos:" );
			//scanf( "%d", &num_elementos );

			//for( i = 0; i < num_elementos; i++ )
			//{
			//	printf( "Ingresa el numero (%d): ", i );
			printf( "Escribe la cadena:\n" );
			scanf( " %[^\n]", cadena );
			//}
			escribirParametro( fd_cadena, cadena );
		}
		else if( opcion[0] == '1' )
		{
			printf( "Escribe el carácter a buscar:\n" );
			scanf( " %s", letra );
			escribirParametro( fd_letra, letra );
		}
		else if( opcion[0] == '3' )
		{
			stop = 1;
		}
		escribirParametro( fd_opcion, opcion );
	}

	close( fd_opcion );
	close( fd_cadena );
	close( fd_letra  );
	return 0;
}

int abrirParametro( char *parametroSysfs )
{
	//char *ruta_parametro= "/sys/module/tarea_1/parameters/";
	//strcat( ruta_parametro, parametroSysfs );
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
