/*
 *  hello-1.c - Módulo de kernel simple.
 */
#include <linux/module.h>	/* Necesaria para todos los modulos */
#include <linux/kernel.h>	/* Necesaria para KERN_INFO */

int init_module(void)
{
	printk(KERN_INFO "Hola mundo, Inicializando el modulo 1.\n");

	/*
	 * Un valor de retorno diferente de 0 significa que
	 * init_module fallo; el modulo no se puede cargar
	 */
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Terminando la ejecucion del modulo 1.\n");
}
