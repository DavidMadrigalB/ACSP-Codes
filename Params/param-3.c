/*
*  para-3.c - Demostración de parámetros en el módulo de kernel
*  se usa un parámetro entero (int), una cadena (charp) y un arreglo(int)
*/
#include <linux/module.h> 	/* Necesaria para todos los módulos 		*/
#include <linux/kernel.h> 	/* Necesaria para KERN_INFRO 			*/
#include <linux/init.h> 	/* Necesaria para las macros de documentación	*/
#include <linux/moduleparam.h> 	/* Necesaria para las macros de parámetros 	*/
#define MAX 5

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Madrigal Buendía");
MODULE_DESCRIPTION("ESTE ES UN EJEMPLO DE MODULO DE KERNEL");
MODULE_INFO(driver, "PULSO CARDIACO");
MODULE_INFO(interfaz, "UART");

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

/* Funciones estáticas solo se pueden mandar a llamar en el mismo módulo de kernel, no en otro */
static int __init funcion_inicio(void)
{
	register int i;
	printk(KERN_INFO "Iniciando el ejemplo 3 de parámetros.\n");
	printk(KERN_INFO "La irq en la función inicio es: %d \n", irq);
	printk(KERN_INFO "El comando en la función inicio es: %s \n", comando);
	printk(KERN_INFO "Número de elementos en la función de inicio: %d\n", numEle);
	for( i = 0; i < numEle; i++ )
	{
		printk(KERN_INFO "Datos[%d] = %d \n", i, datos[i]);
	}
	/*
	* Un valor de retorno diferente de 0 significa que
	* init_module fallo; el modulo no se puede cargar
	*/
	return 0;
}

static void __exit funcion_exit(void)
{
	register int i;

	printk(KERN_INFO "Terminando la ejecución del ejemplo 2 de parámetros.\n");
	printk(KERN_INFO "La irq en la función exit es: %d \n", irq);
	printk(KERN_INFO "El comando en la función exit es: %s \n", comando);
	printk(KERN_INFO "Número de elementos en la función de salida: %d \n", numEle);
	for( i = 0; i < numEle; i++ )
	{
		printk(KERN_INFO "Datos[%d] = %d \n", i, datos[i]);
	}
}

/* Macros */
module_init( funcion_inicio );
module_exit( funcion_exit  );
