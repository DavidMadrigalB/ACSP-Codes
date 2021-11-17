/*
*  thread-1.c - Demostración de la ejecución de un hilo en espacio de kernel
*  se usa la función kthread_create() y wake_up_process()
*/
#include <linux/module.h> 	/* Necesaria para todos los módulos 		*/
#include <linux/kernel.h> 	/* Necesaria para KERN_INFRO 			*/
#include <linux/init.h> 	/* Necesaria para las macros de documentación	*/
#include <linux/moduleparam.h> 	/* Necesaria para las macros de parámetros 	*/
#include <linux/kthread.h>	/* Necesaria para los hilos			*/
#include <linux/delay.h>	/* Necesaria para las funciones de tiempo	*/
#define MAX 5

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Madrigal Buendía");
MODULE_DESCRIPTION("ESTE ES UN EJEMPLO DE MODULO DE KERNEL");
MODULE_INFO(driver, "PULSO CARDIACO");
MODULE_INFO(interfaz, "UART");
/*
static int irq = 10;
module_param( irq, int, 0660 ); //0660: son los permisos en binario
MODULE_PARM_DESC( irq, "Número de interrupción" );

static char *comando = "derecha";
module_param( comando, charp, 0660 );
MODULE_PARM_DESC( comando, "Comando del motor" );

static int datos [MAX] = {1, 2, 3, 4, 5};
static int numEle = MAX;
module_param_array( datos, int, &numEle, 0660 );
MODULE_PARM_DESC( datos, "Muestras" );
*/
struct task_struct *khilo;

static int hilo_kernel( void *arg )
{
	int id= *(int *)arg;
	int cont= 0;

	printk(KERN_INFO "Iniciando la ejecución de la función hilos_kernel \n");

	while( !kthread_should_stop() )//Esperar mientras no se mande a llamar la otra función kthread_stop, mientras no lo detenga
	{
		schedule();
		ssleep(5);
		printk(KERN_INFO "Planificador %d \n", cont++ );
		//ssleep(2); //bloquea el hilo por un numero de segudos = 2
		//msleep(500); //bloquea el hilo por un numero de milisegundos = 500
		//udelay(1000); //bloquea el hilo por un numero de microsegundos = 1000
	}
	return id;
}

/* Funciones estáticas solo se pueden mandar a llamar en el mismo módulo de kernel, no en otro */
static int __init funcion_inicio(void)
{
	static int id_thread= 10;
	
	printk(KERN_INFO "Iniciando el ejemplo 1 de hilos de kenerl \n");
	khilo= kthread_create( hilo_kernel, (void *) &id_thread, "kmi_hilo" );
	
	if( IS_ERR(khilo) ) //Macro que verifica que se creo el hilo
	{
		printk(KERN_ERR "Error al crear hilo");

		return PTR_ERR(khilo);
	}
	wake_up_process( khilo );
	printk(KERN_INFO "Hilo creado con un pid: %d con nombre: %s \n", khilo->pid, khilo->comm);
	
	/*
	* Un valor de retorno diferente de 0 significa que
	* init_module fallo; el modulo no se puede cargar
	*/
	return 0;
}
	
static void __exit funcion_exit(void)
{
	int ret_hilok= kthread_stop( khilo );
	printk(KERN_INFO "Terminando la ejecución del ejemplo 1 de hilos \n");

	if( ret_hilok == -EINTR )
		printk(KERN_ERR "Error en la terminación del hilo \n");
	else
		printk(KERN_ERR "Terminación del hilo con valor: %d \n", ret_hilok);
	//printk(KERN_INFO "La irq en la función exit es: %d \n", irq);
}

/* Macros */
module_init( funcion_inicio );
module_exit( funcion_exit  );
