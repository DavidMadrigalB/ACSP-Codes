#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#define N 1024

int abrirParametro(char *parametroSysfs);
void leerParametro(int fd,char *parametro,int Nbytes);
void escribirParametro(int fd,char *parametro);

int main(){
	int fdComando;
	char comando[N];
	int sel=10;

	fdComando = abrirParametro("/sys/module/callboleta/parameters/comando");

	printf("*****Valor de Datos *****");
	leerParametro(fdComando,comando,N);

	printf("\n\n********OPCIONES******\n");
	printf("1.- Ingresar Boleta (Cadena)\n");
	printf("0.- Salir\n\n");
	printf("Ingresa la opción que quieras realizar -> ");
	scanf("%d",&sel);

	do{
		if(sel==1){
			printf("Ingresa la cadena -> ");
			//fgets(cadena,N,stdin);
			fflush(stdin);
			scanf(" %[^\n]",comando);
			fflush(stdin);
			escribirParametro(fdComando,comando);
			sel=0;
		}
		else if(sel==0){
			exit (0);
		}
	}while(sel!=0);
	///////////////
	close(fdComando);

	return 0;
}

int abrirParametro(char *parametroSysfs){
	int fd;
	fd = open(parametroSysfs,O_RDWR);
	if (fd==-1){
		perror("Error al abrir el parámetro.\n");
		exit(EXIT_FAILURE);
	}
	printf("\nEl descriptor del parametro es: %d",fd);
	printf("\n");
	return fd;
}

void leerParametro(int fd,char *parametro,int Nbytes){
	int nbytes;
	nbytes = read(fd,parametro,Nbytes);
	if(nbytes==-1){
		perror("Error al leer en el parámetro.\n");
		exit(EXIT_FAILURE);
	}
	parametro[nbytes-1]=0;
	printf("\nBytes recibidos del parametro: %d",nbytes);
	printf("\nEl valor del parametro es: %s",parametro);
	printf("\n");
}

void escribirParametro(int fd,char *parametro){

	int nbytes;

	nbytes = write(fd,parametro,strlen(parametro)+1);
	if(nbytes==-1){
		perror("Error al escribir en el parámetro.\n");
		exit(EXIT_FAILURE);
	}
	printf("\nBytes enviados al parametro: %d",nbytes);
	printf("\n");
}







