/*
 *  param-2.c - Demostracion de parametros en el modulo de kernel
 *  se usa un parámetro irq, con funciones call back
 */
#include <linux/module.h>	/* Necesaria para todos los modulos 		*/
#include <linux/kernel.h>	/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>		/* Necesaria para las macros de documentacion	*/
#include <linux/moduleparam.h>	/* Necesaria para las macros de parametros 	*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VICTOR HUGO GARCIA ORTEGA");
MODULE_DESCRIPTION("ESTE ES UN EJEMPLO DE MODULO DE KERNEL");
MODULE_VERSION("4.0");
MODULE_INFO(driver, "PULSO CARDIACO");
MODULE_INFO(interfaz, "UART");

static char *comando = "derecha";
module_param( comando, charp, 0660 );
MODULE_PARM_DESC( comando, "Comando del motor" );

int set_param( const char *val, const struct kernel_param *kp )
{
	printk(KERN_INFO "Funcion call back set_param ejecutada...");

	return 0;
}

int get_param( char *buffer, const struct kernel_param *kp );
{
	printk(KERN_INFO "Funcion call back get_param ejecutada...");

	return 0;
}

const struct kernel_param_ops mis_param_ops =
{
	.set = set_param,
	.get = get_param
};

static int irq = 10;
//module_param( irq, int, 0660 );
module_param_cb( irq, &mis_param_ops, &irq, 0660 );
MODULE_PARM_DESC( irq, "Número de interrupción" );

static int __init funcion_inicio(void)
{
	printk(KERN_INFO "Iniciando el ejemplo 2 de parametros .\n");
	printk(KERN_INFO "La irq en la funcion inicio es: %d \n", irq);
	printk(KERN_INFO "El comando en la funcion inicio es: %s \n", comando);
	/*
	 * Un valor de retorno diferente de 0 significa que
	 * init_module fallo; el modulo no se puede cargar
	 */
	return 0;
}

static void __exit funcion_exit(void)
{
	printk(KERN_INFO "Terminando la ejecucion del ejemplo 2 de parametros.\n");
	printk(KERN_INFO "La irq en la funcion exit es: %d \n", irq);
	printk(KERN_INFO "El comando en la funcion exit es: %s \n", comando);
}

module_init( funcion_inicio );
module_exit( funcion_exit );
