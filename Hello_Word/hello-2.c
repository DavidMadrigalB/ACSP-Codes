/*
*  hello-2.c - Demostración de las macros module_init() y module_exit()
*/
#include <linux/module.h> /* Necesaria para todos los módulos */
#include <linux/kernel.h> /* Necesaria para KERN_INFRO */
#include <linux/init.h> /* Necesaria para las macros  */

/* Funciones estáticas solo se pueden mandar a llamar en el mismo módulo de kernel, no en otro */
static int __init funcion_inicio(void)
{
	printk(KERN_INFO "Hola mundo, Inicializando el módulo 2.\n");
	/*
	* Un valor de retorno diferente de 0 significa que
	* init_module fallo; el modulo no se puede cargar
	*/
	return 0;
}

static void __exit funcion_exit(void)
{
	printk(KERN_INFO "Terminando la ejecución del módulo 2.\n");
}

/* Macros */
module_init( funcion_inicio );
module_exit( funcion_exit  );
