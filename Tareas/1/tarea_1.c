/*
*  tarea_1.c
*/
#include <linux/module.h> 	/* Necesaria para todos los módulos 		*/
#include <linux/kernel.h> 	/* Necesaria para KERN_INFRO 			*/
#include <linux/init.h> 	/* Necesaria para las macros de documentación	*/
#include <linux/moduleparam.h> 	/* Necesaria para las macros de parámetros 	*/
#include <linux/kthread.h>	/* Necesaria para los hilos			*/
#include <linux/delay.h>	/* Necesaria para las funciones de tiempo	*/
#define MAX 8

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Madrigal Buendía");
MODULE_DESCRIPTION("Tarea 1");
MODULE_INFO(Asignatura, "Advanced Computing Systems Programming");
//MODULE_INFO(interfaz, "UART");

struct task_struct *khilo;

/* PARAMETROS */
static int opcion = -1;
module_param( opcion, int, 0660 );
MODULE_PARM_DESC( opcion, "Selecciona la operación a realizar con el arreglo" );

static int numEle = MAX;
static int datos [MAX] = {5, 78, -23, 97, 12, -5, 7, 44};
module_param_array( datos, int, &numEle, 0660 );
MODULE_PARM_DESC( datos, "Contiene los datos enteros" );

static int promedio = 0;
MODULE_PARM_DESC( promedio, "Contiene el promedio de los datos del arreglo" );

static int pares [MAX] = {};
MODULE_PARM_DESC( pares, "Contiene los números pares del arreglo" );

static int hilo_kernel( void *arg )
{
	register int i, j;
	int id= *(int *)arg;
	int auxiliar;

	printk( KERN_INFO "Iniciando la ejecución de la función hilos_kernel \n" );
	/*
	* Opcion:
	*  1 - Promedio de los elementos del arreglo. Imprime el promedio en el archivo LOG
	*  2 - Ordenamiento del arreglo usando burbuja. Este ordenamiento debe quedar en el
	*       mismo arreglo donde se encuentran los datos originales. Imprime el arreglo
	*	ordenado en el archivo LOG
	*  3 - Mostrar los numeros pares del arreglo datos. Imprime los numeros pares en el
	*	archivo LOG
	*/

	// Espera mientras no se mande a llamar la función kthread_stop, hasta que lo detenga
	while( !kthread_should_stop() )
	{
		msleep(1000);
		printk( KERN_INFO "Opción %d \n", opcion );
		if( opcion == 1 )
		{
			promedio = 0;
			for( i = 0; i < numEle; i++ )
			{
				promedio = promedio + datos[i];
			}
			promedio = (promedio / numEle);
			printk( KERN_INFO "Promedio: %d", promedio );
		}
		else if( opcion == 2 )
		{
			for( i = 0; i < numEle; i++ )
			{
				for( j = 0; j < numEle; j++ )
				{
					if( datos[i] < datos[j])
					{
						auxiliar = datos[i];
						datos[i] = datos[j];
						datos[j] = auxiliar;
					}
				}
			}
			for( i = 0; i < numEle; i++ )
			{
				printk( KERN_INFO "Datos[%d] = %d", i, datos[i] );
			}
		}
		else if( opcion == 3 )
		{
			j = 0;
			for( i = 0; i < numEle; i++ )
			{
				if( datos[i] % 2 == 0 )
				{
					pares[j] = datos[i];
					printk( KERN_INFO "Pares[%d] = %d", j, pares[j] );
					j++;
				}
			}
		}
	}
	return id;
}

/* Funciones estáticas solo se pueden mandar a llamar en el mismo módulo de kernel, no en otro */
static int __init funcion_inicio(void)
{
	static int id_thread= 1;

	printk(KERN_INFO "Iniciando la tarea_1 \n");
	khilo= kthread_create( hilo_kernel, (void *) &id_thread, "tarea_1_hilo" );

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
	printk(KERN_INFO "Terminando la ejecución de la tarea_1 \n");

	if( ret_hilok == -EINTR )
		printk(KERN_ERR "Error en la terminación del hilo \n");
	else
		printk(KERN_ERR "Terminación del hilo con valor: %d \n", ret_hilok);
	//printk(KERN_INFO "La irq en la función exit es: %d \n", irq);
}

/* Macros */
module_init( funcion_inicio );
module_exit( funcion_exit  );
