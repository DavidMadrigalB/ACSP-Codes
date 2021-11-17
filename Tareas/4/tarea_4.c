/*
 *  tarea4.c - Programa en módulo kernel que permite a realizar diversas operaciones
 *  con un arreglo entero. A través de funciones callback.
*/

#include <linux/module.h>		/* Necesaria para los módulos 			*/
#include <linux/kernel.h>		/* Necesaria para KERN_INFO			*/
#include <linux/init.h>			/* Necesaria para las macros de documentación	*/
#include <linux/moduleparam.h>		/* Necesaria para las macros de parámetros 	*/

#define MAX 8 	/* Tamaño del arreglo de enteros */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DAVID MADRIGAL BUENDIA");
MODULE_DESCRIPTION("TAREA 4: Manejo de un arreglo de enteros, usando funciones callback");
MODULE_VERSION("1.0");
MODULE_INFO(subject, "ADVANCED COMPUTING SYSTEMS PROGRAMMING");
MODULE_INFO(institute, "INSTITUTO POLITÉCNICO NACIONAL (IPN)");
MODULE_INFO(school, "ESCUELA SUPERIOR DE CÓMPUTO (ESCOM)");

/* PROTOTIPO DE FUNCIONES */
void calcular_promedio(void);
void obtener_pares(void);
void ordenar_arreglo(void);

/* VARIABLES PRINCIPALES */
static int datos[MAX] = {5, 78, -23, 97, 12, -5, 7, 44};
static int opcion = 0;
static int promedio = 0;
static int pares[MAX] = {0};

/* VARIABLES AUXILIARES */
static int num_elementos = MAX;

int set_param_datos( const char *val, const struct kernel_param *kp )
{
/*
	char *arreglo = (char *)kp->arg;
	int longitud = strlen(val);

	printk(KERN_INFO "Función call-back set_param_datos ejecutada... \n");
	printk(KERN_INFO "Parámetro arg: %s, val: %s \n", arreglo, val);

	if( longitud > MAX*2 )
	{
		printk(KERN_INFO "Error el arreglo: %s es muy largo \n", val);
		// Valor máximo del arreglo superado
		return -ENOSPC;
	}
	else if( longitud == 1 )
	{
		printk(KERN_INFO "Error parámetro vacío \n");
		// Valor inválido
		return -EINVAL;
	}

	char *endp = NULL;
	char *start = val;
	long num = 0;
	register int i;

	for( i = 0; i < MAX; i++)
	{
		kstrtol(start, 10, &num);
		*arreglo = (int) num;
		start = endp + 1;
	}

	strcpy( kp->arg, val );


	printk(KERN_INFO "Parámetro datos (después de la copia): %s \n", datos);
*/
	return 0;
}

int get_param_datos( char *buffer, const struct kernel_param *kp )
{
	int *datos_info = (int *)kp->arg;
	int ret;

	printk(KERN_INFO "Función call-back get_param_datos ejecutada... \n");
	printk(KERN_INFO "Parámetro buffer: %s \n", buffer);

	ret = param_get_int( buffer, kp );
	if( ret > 0)
	{
		printk(KERN_INFO "Valores después de la conversión... \n");
		printk(KERN_INFO "Parámetro buffer: %s \n", buffer);
		printk(KERN_INFO "Valor de retorno: %d \n", ret);

		return ret;
	}

	return -EPERM;
}

int set_param_opcion( const char *val, const struct kernel_param *kp )
{
	int param_arg = *(int *)kp->arg;
	int ret;

	printk(KERN_INFO "Función call-back set_param_opcion ejecturada... \n");
	printk(KERN_INFO "Parámetro val: %s \n", val);
	printk(KERN_INFO "Parámetro arg: %d, opcion: %d \n", param_arg, opcion);

	ret = param_set_int( val, kp );
	if( ret == 0 )
	{
		param_arg = *(int *)kp->arg;
		printk(KERN_INFO "Parámetros después de la conversión: \n");
		printk(KERN_INFO "Parámetro arg: %d, opcion: %d \n", param_arg, opcion);

		/*
			Opción:
				1 - Calcular promedio de los elementos del arreglo
					Imprime al promedio en el archivo log
				2 - Ordenar el arreglo usando burbuja
					El ordenamiento del arreglo se hace en la misma variable
					Imprime el arreglo ordenado en el archivo log
				3 - Muestra los números pares del arreglo
					Imprime los números pares en el archivo log
		*/
		if( opcion == 1 ) {
			calcular_promedio();
		}
		else if( opcion == 2 )
		{
			ordenar_arreglo();
		}
		else if( opcion == 3 )
		{
			obtener_pares();
		}
		/*
		else
		{
			Nothing
		}
		*/
	}

	return ret;
}

int get_param_opcion( char *buffer, const struct kernel_param *kp )
{
	int param_arg = *(int *)kp->arg;
	int ret;

	printk(KERN_INFO "Función call-back get_param_opcion ejecutada... \n");
	printk(KERN_INFO "Parámetro buffer: %s \n", buffer);
	printk(KERN_INFO "Parámetro arg: %d \n", param_arg);

	ret = param_get_int( buffer, kp );
	if( ret > 0)
	{
		printk(KERN_INFO "Valores después de la conversión: \n");
		printk(KERN_INFO "Parámetro buffer: %s \n", buffer);
		printk(KERN_INFO "Valor de retorno: %d \n", ret);
	}

	return -EPERM;
}

const struct kernel_param_ops param_ops_datos =
{
	.set = set_param_datos,
	.get = get_param_datos
};

const struct kernel_param_ops param_ops_opcion =
{
	.set = set_param_opcion,
	.get = get_param_opcion
};

module_param_cb( datos, &param_ops_datos, datos, 0660 );
MODULE_PARM_DESC( datos, "Contiene los datos enteros" );

module_param_cb( opcion, &param_ops_opcion, &opcion, 0660 );
MODULE_PARM_DESC( opcion, "Selecciona la operación a realizar con el arreglo" );

MODULE_PARM_DESC( promedio, "Contiene el promedio de los datos del arreglo" );
MODULE_PARM_DESC( pares, "Contiene los números pares del arreglo" );


void calcular_promedio(void)
{
	printk(KERN_INFO "Función para obtener promedio ejecutada... \n");
	register int i = 0;
	promedio = 0;

	for( i = 0; i < num_elementos; i++ )
	{
		promedio = promedio + datos[i];
	}
	promedio = (promedio / num_elementos);

	printk(KERN_INFO "Parámetro promedio: %d \n", promedio);
}

void obtener_pares(void)
{
	printk(KERN_INFO "Función para obtener números pares ejecutada... \n");
	register int i = 0;
	register int j = 0;

	printk(KERN_INFO "Números pares: \n");
	for( i = 0; i < num_elementos; i++ )
	{
		if( datos[i] % 2 == 0 )
		{
			pares[j] = datos[i];

			printk(KERN_INFO "Pares[%d] = %d \n", j, pares[j]);
			j++;
		}
	}
}

void ordenar_arreglo(void)
{
	printk(KERN_INFO "Función para ordenar el arreglo ejecutada... \n");
	register int i = 0;
	register int j = 0;
	register int aux = 0;

	for( i = 0; i < num_elementos; i++ )
	{
		for( j = i+1; j < num_elementos; j++ )
		{
			if( datos[i] > datos[j] )
			{
				aux = datos[i];
				datos[i] = datos[j];
				datos[j] = aux;
			}
		}
	}

	printk(KERN_INFO "Datos ordenados: \n");
	for( i = 0; i < num_elementos; i++ )
	{
		printk(KERN_INFO "Datos[%d] = %d", i, datos[i]);
	}
}

static int __init funcion_inicio(void)
{
	register int i = 0;
	printk(KERN_INFO "Iniciando el programa. \n");

	printk(KERN_INFO "Los datos son:");
	for( i = 0; i < num_elementos; i++)
	{
		printk(KERN_INFO "Datos[%d] = %d", i, datos[i]);
	}

	return 0;
}

static void __exit funcion_exit(void)
{
	register int i = 0;
	printk(KERN_INFO "Terminando el programa. \n");

	printk(KERN_INFO "Los datos son:");
	for( i = 0; i < num_elementos; i++)
	{
		printk(KERN_INFO "Datos[%d] = %d", i, datos[i]);
	}
}

module_init ( funcion_inicio );
module_exit ( funcion_exit );
