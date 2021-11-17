/*
 *  hello-2.c - Demostracion de las macros module_init() y module_exit().
 *  Estas macros se prefieren sobre las funciones init_module() y cleanup_module().
 */

#include <linux/module.h>	/* Necesaria para todos los modulos */
#include <linux/kernel.h>	/* Necesaria para KERN_INFO */
#include <linux/init.h>		/* Necesaria para las macros */

static int __init funcion_inicio(void)
{
	printk(KERN_INFO "Hola mundo en el modulo 2\n");
	return 0;
}

static void __exit funcion_exit(void)
{
	printk(KERN_INFO "Terminando la ejecucion del modulo 2\n");
}

module_init(funcion_inicio);
module_exit(funcion_exit);
