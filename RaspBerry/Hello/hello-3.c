/*
 *  hello-3.c - Demostración de la documentación de un módulo de kernel
 */
#include <linux/module.h>	/* Necesaria para todos los modulos 	*/
#include <linux/kernel.h>	/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>		/* Necesaria para las macros 			*/

MODULE_LICENSE("GPL");							/* Tipo de licencia								*/
MODULE_AUTHOR("VICTOR H GARCIA O");				/* Autor del módulo 							*/
MODULE_DESCRIPTION("Modulo de kernel simple");	/* Descripción de la funcionalidad del módulo 	*/
MODULE_VERSION("1.0");							/* Versión del módulo 							*/
MODULE_INFO(driver, "PULSO CARDIACO"); 			/* Información personalizada del usuario		*/

static int __init funcion_inicio(void)
{
	printk(KERN_INFO "Hola mundo en el modulo 3\n");
	return 0;
}

static void __exit funcion_exit(void)
{
	printk(KERN_INFO "Terminando la ejecucion del modulo 3\n");
}

module_init(funcion_inicio);
module_exit(funcion_exit);
