/*
 *  hello-4.c - Demostración de __initdata
 */
#include <linux/module.h>			/* Necesaria para todos los modulos 	*/
#include <linux/kernel.h>			/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>				/* Necesaria para las macros 			*/

MODULE_LICENSE("GPL");							/* Tipo de licencia								*/
MODULE_AUTHOR("VICTOR H GARCIA O");				/* Autor del módulo 							*/
MODULE_DESCRIPTION("Manejo de macro __init");	/* Descripción de la funcionalidad del módulo 	*/
MODULE_VERSION("1.0");							/* Versión del módulo 							*/
MODULE_INFO(driver, "PULSO CARDIACO"); 			/* Información personalizada del usuario		*/

static int mivar1 __initdata = 10;

static int __init funcion_inicio(void)
{
	printk(KERN_INFO "Hola mundo en el modulo 4, var1 : %d \n", mivar1);
	return 0;
}

static void __exit funcion_exit(void)
{
	printk(KERN_INFO "Terminando la ejecucion del modulo 4\n");
}

module_init(funcion_inicio);
module_exit(funcion_exit);
