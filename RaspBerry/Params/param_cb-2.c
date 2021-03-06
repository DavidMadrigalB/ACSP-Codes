/*
 *  param_cb-2.c - Demostracion de parametros en el modulo de kernel mediante funciones
 *  call back se usa un parametro entero (int) y una cadena (charp)
 */
#include <linux/module.h>	/* Necesaria para todos los modulos 		*/
#include <linux/kernel.h>	/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>		/* Necesaria para las macros de documentacion	*/
#include <linux/moduleparam.h>	/* Necesaria para las macros de parametros 	*/

#define N 10

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VICTOR HUGO GARCIA ORTEGA");
MODULE_DESCRIPTION("ESTE ES UN EJEMPLO DE MODULO DE KERNEL");
MODULE_VERSION("4.0");
MODULE_INFO(driver, "PULSO CARDIACO");
MODULE_INFO(interfaz, "UART");

static char comando[N];
//module_param( comando, charp, 0660 );
//MODULE_PARM_DESC( comando, "Comando del motor" );

static int irq = 10;

int set_param_charp( const char *val, const struct kernel_param *kp )
{
	char *cadena = (char *)kp->arg;
	int longitud = strlen(val);

	printk(KERN_INFO "Funcion call back set_param ejecutada...\n");
	printk(KERN_INFO "Parametro sal: %s \n", val);
	printk(KERN_INFO "Parametro arg: %s, comando: %s \n", cadena, comando);

	if( longitud > N )
	{
		printk(KERN_INFO "Error la cadena: %s es muy larga \n", val);
		return -ENOSPC; //Código de error
		// Se retorna cuando el tamaño del arreglo, es mayor al máximo indicado
	}else if( longitud == 1 ) {
		printk(KERN_INFO "Error parámetro vacío \n");
		return -EINVAL; //Código de error
		// Valor invalido
	}
	strcpy( kp->arg, val );

	printk(KERN_INFO "Parámetro comando después de la copia: %s \n", comando);

	return 0;
}

int get_param_charp( char *buffer, const struct kernel_param *kp)
{
	char *cadena = (char *)kp->arg;
	int longitud;

	printk(KERN_INFO "Función call back get_param_charp ejecutada...\n");
	printk(KERN_INFO "Parámetro buffer: %s \n", buffer);
	printk(KERN_INFO "Parámetro arg: %s, comando: %s", cadena, comando);

	strcpy( buffer, cadena );
	longitud = strlen(buffer);

	printk(KERN_INFO "Parametro buffer despues de la copia: %s \n", buffer);

	return 0;
}

int set_param( const char *val, const struct kernel_param *kp )
{
	int paramArg = *(int *)kp->arg;
	int ret;

	printk(KERN_INFO "Funcion call back set_param ejecutada...\n");
	printk(KERN_INFO "Parametro sal: %s \n", val);
	printk(KERN_INFO "Parametro arg: %d, irq: %d \n", paramArg, irq);

	ret = param_set_int( val, kp );
	if( ret == 0 )
	{
		paramArg = *(int *)kp->arg;
		printk(KERN_INFO "Parametros despues de la conversion:\n");
		printk(KERN_INFO "Parametro arg: %d, irq: %d \n", paramArg, irq);
		//colocamos codigo propio
		/*
		if( irq == 1 )
			promedio();
		else
			pares();
		*/
	}

	return ret;
}

int get_param( char *buffer, const struct kernel_param *kp )
{
	int paramArg = *(int *)kp->arg;
	int ret;

	printk(KERN_INFO "Funcion call back get_param ejecutada...\n");
	printk(KERN_INFO "Parametro buffer: %s \n", buffer);
	printk(KERN_INFO "Parametro arg: %d \n", paramArg);

	ret = param_get_int( buffer, kp );
	if( ret > 0 )
	{
		printk(KERN_INFO "Valores despues de la conversion...\n");
		printk(KERN_INFO "Parametro buffer: %s \n", buffer);
		printk(KERN_INFO "Valor de retorno: %d \n", ret);

		return ret;//Numero de caracteres que tomara para mostrarlo en consola
	}

	return -EPERM;
}

const struct kernel_param_ops mis_param_ops =
{
	.set = set_param,
	.get = get_param
};

const struct kernel_param_ops mis_param_charp_ops =
{
	.set = set_param_charp,
	.get = get_param_charp
};

//module_param( irq, int, 0660 );
module_param_cb( irq, &mis_param_ops, &irq, 0660 );
MODULE_PARM_DESC( irq, "Número de interrupción" );

module_param_cb( comando, &mis_param_charp_ops, comando, 0660 );
MODULE_PARM_DESC( comando, "Comando del motor" );

static int __init funcion_inicio(void)
{
	printk(KERN_INFO "Iniciando el ejemplo de funciones call back\n");
	printk(KERN_INFO "La irq en la funcion inicio es: %d \n", irq);
	strcpy( comando, "arriba" );
	printk(KERN_INFO "El comando en la funcion inicio es: %s \n", comando);
	/*
	 * Un valor de retorno diferente de 0 significa que
	 * init_module fallo; el modulo no se puede cargar
	 */
	return 0;
}

static void __exit funcion_exit(void)
{
	printk(KERN_INFO "Terminando la ejecucion del ejemplo de funciones call back\n");
	printk(KERN_INFO "La irq en la funcion exit es: %d \n", irq);
	printk(KERN_INFO "El comando en la funcion exit es: %s \n", comando);
}

module_init( funcion_inicio );
module_exit( funcion_exit );
