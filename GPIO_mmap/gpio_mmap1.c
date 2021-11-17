#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#define CICLOS 5

//#define ADD_BASE_GPIO 0x3f200000	/* Direción base para Raspberry Pi3 B+ */
#define ADD_BASE_GPIO 0xfe200000	/* Direción base para Raspberry Pi4 B */

volatile uint32_t * obtDirVirtual( off_t dirBase, int *fd );
void configGPIO(volatile uint32_t *gpio_virtual, int gpio, int modo);
void valueGPIO(volatile uint32_t *gpio_virtual, int gpio, int valor);

int main( )
{
	volatile uint32_t *gpio_virtual;

	int fd ; //file descriptor
	register int i;

	gpio_virtual = obtDirVirtual( ADD_BASE_GPIO, &fd );

        //Se configura el BCM GPIO16 como salida
	configGPIO( gpio_virtual, 16, 0 );

	// *(gpio + 1) = (*(gpio + 1) & ~(7 << 18)) | (1 << 18);

        for( i = 0; i < CICLOS; i++ ){
                //Se pone en uno el GPIO 16
		valueGPIO( gpio_virtual, 16, 1 );
                // *(gpio_virtual + 7) = *(gpio_virtual + 7) | (1 << 16);

                sleep(2);

                //Se pone en ceroel GPIO 16
		valueGPIO( gpio_virtual, 16, 0 );
                // *(gpio_virtual + 10) = *(gpio_virtual + 10) | (1 << 16);

                sleep(2);
        }

	close(fd);
	return 0;
}

/**
 * @brief: Esta funcion obtiene la direccion virtual de un GPIO apartir de una direccion base haciendo uso de MMAP
 * @param: dirBase, tipo offset dirección base; el offset se usa en MMAP para agarrar el numero de la dirección
 * @param: fd, file descriptor por referencia; debe estar abierto hasta el final del programa
 */
volatile uint32_t * obtDirVirtual( off_t dirBase, int *fd )
{
	volatile uint32_t *per_virtual; //periferico virtual

	/* Se obtiene un manejador a la memoria física
	 * /dev/mem es un device driver de caracter que contiene una imagen de la
	 * memoria principal de la computadora.
	 *
     	 * Los Bytes de direcciones en /dev/mem son interpretados como direcciones de
	 * la memoria física.
	 * O_RDWR - Bandera para lectura y escritura
	 * O_SYNC - Bandera para sincronia. El archivo es abierto para acceso de I/O sincrono
	 * Cualquier escritura en el despriptor de archivo bloqueara la llamada a proceso hasta
	 * que el dato se haya escrito fisicamente.
	 */

	*fd = open ("/dev/mem", O_RDWR | O_SYNC);
	if( *fd == -1 )
	{
		printf("Error al abrir /dev/mem %s \n", strerror(errno) );
		exit( EXIT_FAILURE );
  	}

	//Se obtiene una pagina de memoria en el espacio de direcciones virtuales
	//a partir del offset 0x3f200000. En este offset se encuentran
	//mapeados los perifericos. Consultar en el FHS en /proc/interrupts
	//El primer parametro indica que la Direccion de inicio del mapa es cero
	//El segundo parametro es el número de bytes a mapear
	//El tercer parametro indica que la pagina puede ser de lectura y escritura
	//El cuarto parametro indica que las modificaciones al mapa seran visibles a otros procesos
	//El quinto parametro es el manejador de lamemoria fisica
	//El sexto parametro es la dirección de inicio dell segmento 
        per_virtual = (uint32_t *)mmap(0, getpagesize(), PROT_READ | PROT_WRITE,
				MAP_SHARED, *fd, dirBase);

        if (per_virtual == MAP_FAILED){
                printf("Error al obtener direcion virtual: %s\n", strerror(errno));
                return -1;
        }
        printf("La direccion virtual es: %x\n", per_virtual);

	return per_virtual;
}

/**
 * @brief: Esta funcion configura un GPIO como entrada o salida
 * @param: gpio_virtual, Direccion de la memoria virtual de GPIOs
 * @param: gpio, Numero de GPIO a configurar
 * @param: modo, 0 - Salida, 1 - Entrada
 */

void configGPIO( volatile uint32_t *gpio_virtual, int gpio, int modo )
{
	int GPFSEL = gpio / 10;
	int bitRango = (gpio % 10);

	bitRango = bitRango << 1 + bitRango; // bitRango = bitRango * 3

	if( modo == 0 ) // Salida
		*(gpio_virtual + GPFSEL) = (*(gpio_virtual + GPFSEL) & ~(7 << bitRango)) | (1 << bitRango);
	else if( modo == 1 ) // Entrada
		*(gpio_virtual + GPFSEL) = (*(gpio_virtual + GPFSEL) & ~(7 << bitRango));
}

/**
 * @brief: Esta funcion coloca un valor de salida en un GPIO
 * @param: gpio_virtual, Direccion de la memoria virtual de GPIOs
 * @param: gpio, Numero de GPIO a configurar
 * @param: valor, 0 o 1
 * Hay un bit para cada GPIO en el GPSET0, salida; también para los registros clear GPCLR0
 */
void valueGPIO( volatile uint32_t *gpio_virtual, int gpio, int valor )
{
	int bit = (gpio >> 5); // gpio = gpio / 32
	int GPSET = bit + 7; // offset comienza de 7
	int GPCLR = bit + 10; // Se encuentran a partir del offset 10

	int bitRango = gpio % 32;

	if( valor == 1 )
		*(gpio_virtual + GPSET) = *(gpio_virtual + GPSET) | (1 << bitRango);
	else if( valor == 0 )
		*(gpio_virtual + GPCLR) = *(gpio_virtual + GPCLR) | (1 << bitRango);
}
