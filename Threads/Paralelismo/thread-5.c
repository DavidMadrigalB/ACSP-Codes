/*
 *  thread-5.c - Demostracion de la ejecución de un hilo en espacio de kernel
 *  se usa la función kthread_create() y wake_up_process(). Se vinvula el hilo
 *  a un procesador con la funcion kthread_bind().
 *  Se procesa de forma paralela el producto de dos arreglos
 *  C[i] = A[i] * B[i]
 */
#include <linux/module.h>	/* Necesaria para todos los modulos 		*/
#include <linux/kernel.h>	/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>		/* Necesaria para las macros de documentacion	*/
#include <linux/moduleparam.h>	/* Necesaria para las macros de parametros 	*/
#include <linux/kthread.h>	/* Necesaria para los hilos 			*/
#include <linux/delay.h>	/* Necesaria para las funciones de tiempo	*/
#define MAX 16
#define NUM_HILOS 4

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VICTOR HUGO GARCIA ORTEGA");
MODULE_DESCRIPTION("ESTE ES UN EJEMPLO DE MODULO DE KERNEL");
MODULE_VERSION("4.0");
MODULE_INFO(driver, "PULSO CARDIACO");
MODULE_INFO(interfaz, "UART");

static int A[MAX] = {12, 45, 87, 38, 97, -45, 44, 88, 22, 43, -5, -23, -34,  -8, 17, 20};
static int B[MAX] = {66,  3, 48, 23, 21,   9,  4,  8,  2, -4, -8, -20, -10, -56, 88, 54};
static int C[MAX];

struct task_struct *khilo[NUM_HILOS];

static int hilo_kernel( void *arg )
{
	int nucleo = *(int *)arg;
	int cont = 0, cpu;
	int eleBloque = MAX / NUM_HILOS;
	int iniBloque = nucleo * eleBloque;
	int finBloque = iniBloque + eleBloque;
	register int i;

	printk(KERN_INFO "Iniciando la ejecucion de la funcion hilo_kernel\n");

	i = iniBloque;
	while( !kthread_should_stop() )
	{
		schedule();
		msleep(500);
		for( ;i < finBloque; i++ )
		{
			C[i] = A[i] * B[i];
		}
		cpu = smp_processor_id();
		printk(KERN_INFO "Ejecucion en CPU: %d contador: %d \n", cpu, cont++);
	}

	return nucleo;
}

static int __init funcion_inicio(void)
{
	register int nh;
	static int ids_thread[NUM_HILOS];

	printk(KERN_INFO "Iniciando el ejemplo 5 de HPC usando hilos de kernel\n");

	for( nh = 0; nh < NUM_HILOS; nh++ )
	{
		ids_thread[nh] = nh;
		khilo[nh] = kthread_create( hilo_kernel, (void *)&ids_thread[nh], "kmi_hilo" );

		if( IS_ERR(khilo[nh]) )
		{
			printk(KERN_ERR "Error al crear el hilo de kernel\n");

			return PTR_ERR(khilo[nh]);
		}
		//kthread_bind( khilo[nh], nh );
		wake_up_process( khilo[nh] );
		printk(KERN_INFO "Hilo creado con pid: %d y nombre: %s\n", khilo[nh]->pid, khilo[nh]->comm);
	}
	/*
	 * Un valor de retorno diferente de 0 significa que
	 * init_module fallo; el modulo no se puede cargar
	 */
	return 0;
}

static void __exit funcion_exit(void)
{
	int ret_hilok;
	register int i;

	printk(KERN_INFO "Terminando la ejecucion del ejemplo 5 de HPC con hilos\n");

	for( i = 0; i < NUM_HILOS; i++ )
	{
		ret_hilok = kthread_stop( khilo[i] );

		if( ret_hilok == -EINTR )
			printk(KERN_ERR "Error en la terminacion del hilo\n");
		else
			printk(KERN_INFO "Terminacion del hilo con valor: %d \n", ret_hilok);
	}
	for( i = 0; i < MAX; i++ )
		printk(KERN_INFO "C[%d]: %d \n", i, C[i]);

}

module_init( funcion_inicio );
module_exit( funcion_exit );
