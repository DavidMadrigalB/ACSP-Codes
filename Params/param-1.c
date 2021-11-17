/*
*  para-1.c - Demostración de parámetros en el módulo de kernel
*  se usa un parámetro entero (int)
*/
#include <linux/module.h> 	/* Necesaria para todos los módulos 		*/
#include <linux/kernel.h> 	/* Necesaria para KERN_INFRO 			*/
#include <linux/init.h> 	/* Necesaria para las macros de documentación	*/
#include <linux/moduleparam.h> 	/* Necesaria para las macros de parámetros 	*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Madrigal Buendía");
MODULE_DESCRIPTION("ESTE ES UN EJEMPLO DE MODULO DE KERNEL");
MODULE_INFO(driver, "PULSO CARDIACO");
MODULE_INFO(interfaz, "UART");

static int irq = 10;
module_param( irq, int, 0660 ); //0660: son los permisos en binario
MODULE_PARM_DESC( irq, "Número de interrupción" );

/* Funciones estáticas solo se pueden mandar a llamar en el mismo módulo de kernel, no en otro */
static int __init funcion_inicio(void)
{
	printk(KERN_INFO "Iniciando el ejemplo 1 de parámetros.\n");
	printk(KERN_INFO "La irq en la función inicio es: %d \n", irq);
	/*
	* Un valor de retorno diferente de 0 significa que
	* init_module fallo; el modulo no se puede cargar
	*/
	return 0;
}

static void __exit funcion_exit(void)
{
	printk(KERN_INFO "Terminando la ejecución del ejemplo 1.\n");
	printk(KERN_INFO "La irq en la función exit es: %d \n", irq);
}

/* Macros */
module_init( funcion_inicio );
module_exit( funcion_exit  );
