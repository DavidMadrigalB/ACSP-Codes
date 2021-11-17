/*
*  tarea_2.c - Programar un modulo de kernel que permita realizar diversas operaciones
*  con un arreglo de caracteres
*/
#include <linux/module.h> 	/* Necesaria para todos los módulos 		*/
#include <linux/kernel.h> 	/* Necesaria para KERN_INFO 			*/
#include <linux/init.h> 	/* Necesaria para las macros de documentación	*/
#include <linux/moduleparam.h> 	/* Necesaria para las macros de parámetros 	*/
#include <linux/kthread.h>	/* Necesaria para los hilos			*/
#include <linux/delay.h>	/* Necesaria para las funciones de tiempo	*/
//#define MAX 8

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Madrigal Buendía");
MODULE_DESCRIPTION("Tarea 2");
MODULE_INFO(Asignatura, "Advanced Computing Systems Programming");
//MODULE_INFO(interfaz, "UART");

struct task_struct *khilo;

/* PARAMETROS */
static int opcion = -1;
module_param( opcion, int, 0660 );
MODULE_PARM_DESC( opcion, "Selecciona la operación a realizar con el arreglo" );

//static int numEle = 63;//63
static char *cadena = "esta es la optativa de advanced computing systems programming";
module_param( cadena, charp, 0660 );
MODULE_PARM_DESC( cadena, "Contiene la cadena de caracteres" );

static char *letra = " ";
module_param( letra, charp, 0660 );
MODULE_PARM_DESC( letra, "Contiene la letra a buscar en la cadena" );

static int longitud = 0;
MODULE_PARM_DESC( longitud, "Contiene la longitud de la cadena" );

static int hilo_kernel( void *arg )
{
	register int i;
	int id= *(int *)arg;
	int contador;

	printk( KERN_INFO "Iniciando la ejecución de la función hilos_kernel \n" );
	/*
	* Opcion:
	*  1 - Cuenta cuantas veces aparece el parámetro carácter en el parámetro cadena.
	*	Imprime el número de coincidencias en el archivo log.
	*  2 - Determina la longitud del parámetro cadena. Imprime la longitud en el archivo
	*	log.
	*/

	// Espera mientras no se mande a llamar la función kthread_stop, hasta que lo detenga
	while( !kthread_should_stop() )
	{
		msleep(1000);
		printk( KERN_INFO "Opción %d \n", opcion );
		if( opcion == 1 )
		{
			contador = 0;
			for( i = 0; cadena[i] != '\0'; i++ )
			{
				if( *(cadena + i) == *letra )
				{
					contador = contador + 1;
				}
			}
			printk( KERN_INFO "Veces que aparece el carácter (%s): %d", letra, contador );
		}
		else if( opcion == 2 )
		{
			longitud = 0;
			for( i = 0; cadena[i] != '\0'; i++ )
			{
				longitud = longitud + 1;
			}
			printk( KERN_INFO "Longitud = %d", longitud );
		}
	}
	return id;
}

/* Funciones estáticas solo se pueden mandar a llamar en el mismo módulo de kernel, no en otro */
static int __init funcion_inicio(void)
{
	static int id_thread= 1;

	printk(KERN_INFO "Iniciando la tarea_2 \n");
	khilo= kthread_create( hilo_kernel, (void *) &id_thread, "tarea_2_hilo" );

	if( IS_ERR(khilo) ) //Macro que verifica que se creo el hilo
	{
		printk(KERN_ERR "Error al crear hilo");
		return PTR_ERR(khilo);
	}

	kthread_bind( khilo, 3 );
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
	printk(KERN_INFO "Terminando la ejecución de la tarea_2 \n");

	if( ret_hilok == -EINTR )
		printk(KERN_ERR "Error en la terminación del hilo \n");
	else
		printk(KERN_ERR "Terminación del hilo con valor: %d \n", ret_hilok);
}

/* Macros */
module_init( funcion_inicio );
module_exit( funcion_exit  );
