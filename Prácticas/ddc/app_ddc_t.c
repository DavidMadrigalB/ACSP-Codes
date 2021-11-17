#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#define N 1024

int main(){
	int fd;

	fd = open("dev/ESCOM_device",0_RDWR);
	if(fd == -1){
		perror("Error al abrir el DDC \n");
		exit(EXIT_FAILURE)
	}
	printf("Dame el mensaje: ");
	scanf("%s",buffer);
	write(fd,buffer,strlen(buffer)+1);

	sleep(5);

	read(fd,buffer_read,N);
	printf("Mensaje recibido el DDC: %d \n",buffer_read);

	sleep(5);
	close(fd);

	return 0;
}







