/*
 *  ddc-2.c - Asignación de número mayor y número menor
 *  es un dispositivo de caracter, se usa asignación dinámica
 */
#include <linux/module.h>			/* Necesaria para todos los modulos 		*/
#include <linux/kernel.h>			/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>				/* Necesaria para las macros 			*/
#include <linux/kdev_t.h>			/* Necesaria para las macros MAJOR, MINOR 	*/
#include <linux/fs.h>				/* Necesaria para las funciones de registro	 */

MODULE_LICENSE("GPL");				/* Tipo de licencia				*/
MODULE_AUTHOR("VICTOR H GARCIA O");		/* Autor del módulo 				*/
MODULE_DESCRIPTION("Manejo de macro __init");	/* Descripción de la funcionalidad del módulo 	*/
MODULE_VERSION("1.0");				/* Versión del módulo				*/
MODULE_INFO(driver, "PULSO CARDIACO"); 		/* Información personalizada del usuario	*/

dev_t dispositivo;

static int __init funcion_inicio(void)
{
	int ret;

	printk(KERN_INFO "Iniciando el DDC \n");
	printk(KERN_INFO "Registrando la variable d ecaracter \n");

	ret = alloc_chrdev_region( &dispositivo, 0, 1, "ESCOM_DDC" );
	if( ret < 0 )
	{
		printk(KERN_INFO "Error al registrar el device driver de caracter \n");
	}

	printk(KERN_INFO "Dispositivo Registrado exitosamente... \n");
	printk(KERN_INFO "Número mayor asignado: %d", MAJOR(dispositivo));
	printk(KERN_INFO "Número menor asignado: %d", MINOR(dispositivo));

	return 0;
}

static void __exit funcion_exit(void)
{
	printk(KERN_INFO "Terminando la ejecucion del DDC \n");

	unregister_chrdev_region( dispositivo, 1 );
}

module_init(funcion_inicio);
module_exit(funcion_exit);
