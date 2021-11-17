/** @brief: Este programa muestra números en un display
	En este caso una boleta que contiene 12 dígitos
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <asm/io.h>

//#define ADDR_BASE_GPIO 0xf3200000		//Direccion base para Raspberry Pi3 B+
#define ADDR_BASE_GPIO 0xfe200000		//Direccion base para Raspberry Pi4 B
#define MAX 10

MODULE_LICENSE("GPL");				/* Tipo de licencia				*/
MODULE_AUTHOR("C&D");		/* Autor del módulo 				*/
MODULE_DESCRIPTION("Hilo para display");	/* Descripción de la funcionalidad del módulo 	*/
MODULE_INFO(driver, "Tarea boleta"); 		/* Información personalizada del usuario	*/
MODULE_VERSION("1.0");				/* Versión del módulo 				*/

//static int boleta[MAX] = {2,0,1,5,0,1,0,2,0,6};
//static int numEle=MAX;
//module_param_array(boleta,int,&numEle,0660);
//MODULE_PARM_DESC(boleta,"No. de boleta");


static char *boleta = "0,1,2,3,4,5,6,7,8,9";
module_param( boleta, charp, 0660 );
MODULE_PARM_DESC( boleta, "No. de Boleta" );


void configGPIO( volatile uint32_t *gpio_virtual, int gpio, int modo );
void valueGPIO( volatile uint32_t *gpio_virtual, int gpio, int valor );
void dibujanum( char num );


struct task_struct *khilo;
volatile uint32_t *gpio_virtual;
//int gpios[7]={14,15,18,23,24,25,8};

int gpios[8] = { 14, 15, 18, 23, 24, 25, 8, 7 };
// Caodo (a,b,c,d,e,f,g)
int dig0 [7] = { 1, 1, 0, 1, 0, 1, 1 };
int dig1 [7] = { 0, 1, 1, 0, 0, 0, 0 };
int dig2 [7] = { 1, 1, 0, 1, 1, 0, 1 };
int dig3 [7] = { 1, 1, 1, 1, 0, 0, 1 };
int dig4 [7] = { 0, 1, 1, 0, 0, 1, 1 };
int dig5 [7] = { 1, 0, 1, 1, 0, 1, 1 };
int dig6 [7] = { 1, 0, 1, 1, 1, 1, 1 };
int dig7 [7] = { 1, 1, 1, 0, 0, 0, 0 };
int dig8 [7] = { 1, 1, 1, 1, 1, 1, 1 };
int dig9 [7] = { 1, 1, 1, 1, 0, 1, 1 };

int guion [7] = { 0, 0, 0, 0, 0, 0, 1 };


//e, d, ., c, g, dig4, b
//dig3, dig2, f, a, dig1 <-

/*
 * La funcion que es ejecutada por el hilo del kernel
 * Valor de retorno:
 * Regresa id
 */

int hilo_kernel(void *data){
	int id = *(int *)data;
	register int j;

	printk(KERN_INFO "Hilo en ejecucion con id: %d...\n", id);
    	while( !kthread_should_stop() ){
		j= 0;
		while(boleta[j]!='\0'){
			printk(KERN_INFO "Boleta: %s", boleta);
			dibujanum(boleta[j]);
               		ssleep(1);
			j++;
		}
    	}
	return id;
}
/*
 * La funcion kthread_run crea y despierta a un hilo, lo pone en ejecucion.
 * Ejecuta una llamada a kthread_create() seguida de una llamada a wake_up_process()
 * Valor de retorno:
 * 		Regresa el hilo creado ó
 * 		ERR_PTR(-ENOMEM )
 */

static int __init kernel_init(void){
	static int id_thread = 10;
	register int i;
	printk( KERN_INFO "\n--------------------------------------------\n" );

   	khilo = kthread_create( hilo_kernel, (void *)&id_thread, "kmi_hilo" );

	if( IS_ERR(khilo) ){
		printk("KERN_ERR Error en la creación del hilo... \n");
		return PTR_ERR(khilo);
	}
	//RANGO DE DIRECCIONES DE LOS REGISTROS A PARTIR DE LA DIRECCION BASE
	gpio_virtual = (uint32_t *)ioremap( ADDR_BASE_GPIO, 0X30 );

	for( i = 0; i < 7; i++ ){
		configGPIO(gpio_virtual,gpios[i],0);
	}

	wake_up_process( khilo );
   	printk( KERN_INFO "Hilo de Kernel creado: %s con PID %d \n", khilo->comm, khilo->pid );

   	return 0;
}
/*
 * La funcion kthread_stop realiza lo siguiente:
 * 1. Activa kthread->kthread_should_stop = true
 * 2. Despierta al hilo
 * 3. Espera a que el hilo termine
 * 4. Valor de retorno:
 * 		Regresa el resultado de hilo_kernel ó
 * 		-EINTR si no se llama a la funcion wake_up_process()
 */
static void __exit kernel_exit(void){
	int ret_hilo_kernel = kthread_stop( khilo );

	if( ret_hilo_kernel == -EINTR )
		printk( KERN_INFO "Error en la terminación del hilo \n" );
	else
		printk( KERN_INFO "Hilo de Kernel con id: %d detenido\n", ret_hilo_kernel );

	iounmap( gpio_virtual );
}




void valueGPIO( volatile uint32_t *gpio_virtual, int gpio, int valor ){
	int bit = (gpio >> 5); //gpio / 32
	int GPSET = bit + 7;
	int GPCLR = bit + 10;
	int bitRango = gpio % 32;

	if( valor == 1 )
        	*(gpio_virtual + GPSET) = (1 << bitRango);
	else if( valor == 0 )
        	*(gpio_virtual + GPCLR) = (1 << bitRango);

}

void configGPIO( volatile uint32_t *gpio_virtual, int gpio, int modo ){
	int GPFSEL = gpio / 10;
	int bitRango = (gpio % 10);

	bitRango = bitRango*3; // bitRango = bitRango * 3

	if( modo == 0 )
        	*(gpio_virtual + GPFSEL) = (*(gpio_virtual + GPFSEL) & ~(7 << bitRango)) | (1 << bitRango);
	else if( modo == 1 )
        	*(gpio_virtual + GPFSEL) = (*(gpio_virtual + GPFSEL) & ~(7 << bitRango));
}

//e, d, ., c, g, dig4, b
//dig3, dig2, f, a, dig1 <-
void dibujanum( char num ){
	register int i= 0;

	printk(KERN_INFO "Número impreso: %c", num);
	switch (num){
		case '0':
			printk(KERN_INFO "Es cero");
			for( i = 0; i < 7; i++){
				valueGPIO(gpio_virtual, gpios[i], dig0[i]);
			}
			break;
		case '1':
			printk(KERN_INFO "Es uno");
			for( i = 0; i < 7; i++){
				valueGPIO(gpio_virtual, gpios[i], dig1[i]);
			}
			break;
		case '2':
			printk(KERN_INFO "Es dos");
			for( i = 0; i < 7; i++){
				valueGPIO(gpio_virtual, gpios[i], dig2[i]);
			}
			break;
		case '3':
			printk(KERN_INFO "Es tres");
			for( i = 0; i < 7; i++){
				valueGPIO(gpio_virtual, gpios[i], dig3[i]);
			}
			break;
		case '4':
			printk(KERN_INFO "Es cuatro");
			for( i = 0; i < 7; i++){
				valueGPIO(gpio_virtual, gpios[i], dig4[i]);
			}
			break;
		case '5':
			printk(KERN_INFO "Es cinco");
			for( i = 0; i < 7; i++){
				valueGPIO(gpio_virtual, gpios[i], dig5[i]);
			}
			break;
		case '6':
			printk(KERN_INFO "Es seis");
			for( i = 0; i < 7; i++){
				valueGPIO(gpio_virtual, gpios[i], dig6[i]);
			}
			break;
		case '7':
			printk(KERN_INFO "Es siete");
			for( i = 0; i < 7; i++){
				valueGPIO(gpio_virtual, gpios[i], dig7[i]);
			}
			break;
		case '8':
			printk(KERN_INFO "Es ocho");
			for( i = 0; i < 7; i++){
				valueGPIO(gpio_virtual, gpios[i], dig8[i]);
			}
			break;
		case '9':
			printk(KERN_INFO "Es nueve");
			for( i = 0; i < 7; i++){
				valueGPIO(gpio_virtual, gpios[i], dig9[i]);
			}
			break;
		default:
			printk(KERN_INFO "Es guion (undefined)");
			for( i = 0; i < 7; i++){
				valueGPIO(gpio_virtual, gpios[i], guion[i]);
			}
			break;
	}
}

module_init(kernel_init);
module_exit(kernel_exit);
