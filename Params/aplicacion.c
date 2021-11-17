#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define N 1024

int abrirParametro ( char *parametroSysfs );
void leerParametro ( int fd, char *parametro, int Nbytes );
void escribirParametro (int fd, char *parametro );

int main()
{
	int fdIrq, fdComando, fdDatos, nbytes; //file_descriptor
	//char *irq= "2";
	char irq[N], comando[N], datos[N];

	fdIrq= abrirParametro("/sys/module/param_3/parameters/irq");
	fdComando= abrirParametro("/sys/module/param_3/parameters/comando");
	fdDatos= abrirParametro("/sys/module/param_3/parameters/datos");

	leerParametro( fdIrq, irq, N );
	leerParametro( fdComando, comando, N );
	leerParametro( fdDatos, datos, N );

	strcpy( irq, "7" );
	escribirParametro( fdIrq, irq );
	strcpy( comando, "atras" );
	escribirParametro( fdComando, comando );
	strcpy( datos, "4,5,6,7" );
	escribirParametro( fdDatos, datos );

	close( fdIrq );
	close( fdComando );
	close( fdDatos );

	return 0;
}

int abrirParametro(char *parametroSysfs)
{
	int fd;
	fd= open( parametroSysfs, O_RDWR );
	if( fd == -1 )
	{
		perror("Error al abrir el parametro\n");
		exit(EXIT_FAILURE);
	}
	printf("El descriptor del parametro es: %d\n", fd);
	return fd;
}

void leerParametro(int fd, char *parametro, int Nbytes)
{
	int nbytes;
	nbytes= read( fd, parametro, Nbytes );
	if( nbytes == -1 )
	{
		perror("Error al leer el parametro\n");
		exit(EXIT_FAILURE);
	}
	//Esto es debido al dato basura, que sale al final de la lectura
	parametro[nbytes-1]= 0;
	printf("Bytes recibidos del parametro: %d\n", nbytes);
	printf("El valor del parametro es: %s\n", parametro);
}

void escribirParametro( int fd, char *parametro)
{
	int nbytes;
	//El +1 despues del strlen es por el siguiente del nulo
	//Esto se hace para que no ocurra el dato basura entonces se pone el dato basura ya que hace
	//la conversión de atoi, que es convertir de string a int, se detiene con un nulo
	//Si ocurre un fallo de estos al momento de la conversión el fallo es muy crítico y hay un error en el kernel
	nbytes= write( fd, parametro, strlen(parametro) + 1 ); //System call
	if( nbytes == -1 )
	{
		perror("Error al escribir en el parametro\n");
		exit(EXIT_FAILURE);
	}
	printf("Bytes enviados al parametro: %d\n", nbytes);
}
