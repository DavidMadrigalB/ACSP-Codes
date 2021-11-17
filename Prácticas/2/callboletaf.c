
#include <linux/module.h>	/* Necesaria para todos los modulos 		*/
#include <linux/kernel.h>	/* Necesaria para KERN_INFO 			*/
#include <linux/init.h>		/* Necesaria para las macros de documentacion	*/
#include <linux/moduleparam.h>	/* Necesaria para las macros de parametros 	*/
#include <linux/sched.h>
#include <linux/delay.h>
#include <asm/io.h>
#define ADDR_BASE_GPIO 0xfe200000
#define N 10

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VICTOR HUGO GARCIA ORTEGA");
MODULE_DESCRIPTION("ESTE ES UN EJEMPLO DE MODULO DE KERNEL");
MODULE_VERSION("4.0");
MODULE_INFO(driver, "PULSO CARDIACO");
MODULE_INFO(interfaz, "UART");

void configGPIO( volatile uint32_t *gpio_virtual, int gpio, int modo );
void valueGPIO( volatile uint32_t *gpio_virtual, int gpio, int valor );
void dibujanum(int num);

struct task_struct *khilo;
volatile uint32_t *gpio_virtual;
int gpios[7]={14,15,18,23,24,25,8};

static char comando[N];
static int irq = 10;


int set_param( const char *val, const struct kernel_param *kp)
{

	int paramArg = *(int *)kp->arg;
	int ret;

	printk(KERN_INFO "Funciòn Call Back set_param ejecutada ... \n");
	printk(KERN_INFO "parametro val: %s \n",val);
	printk(KERN_INFO "parametro arg: %d, irq: %d \n",paramArg,irq);

	//Esta funcion convierte una cadena a un entero
	ret = param_set_int(val,kp);
	if(ret == 0)
	{
		paramArg = *(int *)kp->arg;
		printk(KERN_INFO "valores despues de la conversion ... \n");
		printk(KERN_INFO "Parametro arg: %d,irq: %d \n",paramArg,irq);
		//colocamos código propio
	}
	return ret;
}


int get_param( char *buffer, const struct kernel_param *kp)
{

	int paramArg = *(int *)kp->arg;	
	int ret;

	printk(KERN_INFO "Funciòn Call Back get_param ejecutada ... \n");
	printk(KERN_INFO "Parametro buffer: %s \n",buffer);
	printk(KERN_INFO "parametro arg: %d, irq: %d \n",paramArg,irq);

	ret = param_get_int(buffer,kp);

	if(ret > 0)
	{
		printk(KERN_INFO "valores despues de la conversion ... \n");
		printk(KERN_INFO "Parametro buffer: %s \n",buffer);
		printk(KERN_INFO "Valor de Retorno: %d \n",ret);
		return ret;
	}

	return -EPERM;
}


int set_param_charp( const char *val, const struct kernel_param *kp)
{

	char *cadena = (char *)kp->arg;	
	int longitud = strlen(val);
	register int j;

	printk(KERN_INFO "Funciòn Call Back set_param_charp ejecutada ... \n");
	printk(KERN_INFO "Parametro val: %s \n",val);
	printk(KERN_INFO "Parametro arg: %s, comando: %s \n",cadena,comando);

	if(longitud > N)
	{
		printk(KERN_ERR "Error la cadena %s es muy larga \n",val);
		return -ENOSPC;
	}else if(longitud == 1)
	{
		printk(KERN_ERR "Error parámetro vacio\n");
		return -EINVAL;
	}
	strcpy(kp->arg,val);
	//Codigo propio
	printk(KERN_INFO "Parametro comando después de la copia: %s\n",comando);

	j=0;
	while(comando[j]!='\0'){
		dibujanum(comando[j]);
               	ssleep(1);
		j++;
	}

	return 0;
}


int get_param_charp( char *buffer, const struct kernel_param *kp)
{

	char *cadena = (char *)kp->arg;
	int longitud;

	printk(KERN_INFO "Funciòn Call Back get_param_charp ejecutada ... \n");
	printk(KERN_INFO "Parametro buffer: %s \n",buffer);
	printk(KERN_INFO "Parametro arg: %s, comando: %s \n",cadena,comando);

	strcpy(buffer, cadena);
	longitud = strlen(buffer);

	printk(KERN_INFO "Parametro buffer después de la copia: %s\n",buffer);

	return longitud;
}


static const struct kernel_param_ops mis_param_ops =
{
	.set = set_param,
	.get = get_param
};

static const struct kernel_param_ops mis_param_charp_ops =
{
	.set = set_param_charp,
	.get = get_param_charp
};


module_param_cb( irq, &mis_param_ops, &irq, 0660);
MODULE_PARM_DESC( irq, "Número de interrupción" );

module_param_cb( comando, &mis_param_charp_ops, comando, 0660 );
MODULE_PARM_DESC( comando, "Comando del motor" );


static int __init funcion_inicio(void)
{
	register int i;
	printk(KERN_INFO "Iniciando el ejemplo de funciones call back .\n");
	printk(KERN_INFO "La irq en la funcion inicio es: %d \n", irq);
	strcpy(comando, "0123456789");
	printk(KERN_INFO "El comando en la funcion inicio es: %s \n", comando);
	gpio_virtual = (uint32_t *)ioremap( ADDR_BASE_GPIO, 0X30 );

	for(i=0;i<7;i++){
		configGPIO(gpio_virtual,gpios[i],0);
	}
	return 0;
}

static void __exit funcion_exit(void)
{
	printk(KERN_INFO "Terminando la ejecucion del ejemplo de funciones call back.\n");
	printk(KERN_INFO "La irq en la funcion exit es: %d \n", irq);
	printk(KERN_INFO "El comando en la funcion exit es: %s \n", comando);
	iounmap( gpio_virtual );
}

void valueGPIO( volatile uint32_t *gpio_virtual, int gpio, int valor ){
	int bit = (gpio >> 5);
	int GPSET = bit + 7;
	int GPCLR = bit + 10;
	int bitRango = gpio % 32;

	if( valor == 1 )
        	*(gpio_virtual + GPSET) = (1 << bitRango);
	else if( valor == 0 )
        	*(gpio_virtual + GPCLR) = (1 << bitRango);

}

void configGPIO( volatile uint32_t *gpio_virtual, int gpio, int modo ){
	int GPFSEL = gpio / 10;
	int bitRango = (gpio % 10);

	bitRango = bitRango*3; // bitRango = bitRango * 3

	if( modo == 0 )
        	*(gpio_virtual + GPFSEL) = (*(gpio_virtual + GPFSEL) & ~(7 << bitRango)) | (1 << bitRango);
	else if( modo == 1 )
        	*(gpio_virtual + GPFSEL) = (*(gpio_virtual + GPFSEL) & ~(7 << bitRango));
}

void dibujanum(int num){
	if (num == 0){
		valueGPIO(gpio_virtual,gpios[0],1);
		valueGPIO(gpio_virtual,gpios[1],1);
		valueGPIO(gpio_virtual,gpios[2],1);
		valueGPIO(gpio_virtual,gpios[3],1);
		valueGPIO(gpio_virtual,gpios[4],1);
		valueGPIO(gpio_virtual,gpios[5],1);
		valueGPIO(gpio_virtual,gpios[6],0);
	}
	else if(num == 1){
		valueGPIO(gpio_virtual,gpios[0],0);
		valueGPIO(gpio_virtual,gpios[1],1);
		valueGPIO(gpio_virtual,gpios[2],1);
		valueGPIO(gpio_virtual,gpios[3],0);
		valueGPIO(gpio_virtual,gpios[4],0);
		valueGPIO(gpio_virtual,gpios[5],0);
		valueGPIO(gpio_virtual,gpios[6],0);
	}
	else if(num == 2){
		valueGPIO(gpio_virtual,gpios[0],1);
		valueGPIO(gpio_virtual,gpios[1],1);
		valueGPIO(gpio_virtual,gpios[2],0);
		valueGPIO(gpio_virtual,gpios[3],1);
		valueGPIO(gpio_virtual,gpios[4],1);
		valueGPIO(gpio_virtual,gpios[5],0);
		valueGPIO(gpio_virtual,gpios[6],1);
	}
	else if(num == 3){
		valueGPIO(gpio_virtual,gpios[0],1);
		valueGPIO(gpio_virtual,gpios[1],1);
		valueGPIO(gpio_virtual,gpios[2],1);
		valueGPIO(gpio_virtual,gpios[3],1);
		valueGPIO(gpio_virtual,gpios[4],0);
		valueGPIO(gpio_virtual,gpios[5],0);
		valueGPIO(gpio_virtual,gpios[6],1);
	}
	else if(num == 4){
		valueGPIO(gpio_virtual,gpios[0],0);
		valueGPIO(gpio_virtual,gpios[1],1);
		valueGPIO(gpio_virtual,gpios[2],1);
		valueGPIO(gpio_virtual,gpios[3],0);
		valueGPIO(gpio_virtual,gpios[4],0);
		valueGPIO(gpio_virtual,gpios[5],1);
		valueGPIO(gpio_virtual,gpios[6],1);
	}
	else if(num == 5){
		valueGPIO(gpio_virtual,gpios[0],1);
		valueGPIO(gpio_virtual,gpios[1],0);
		valueGPIO(gpio_virtual,gpios[2],1);
		valueGPIO(gpio_virtual,gpios[3],1);
		valueGPIO(gpio_virtual,gpios[4],0);
		valueGPIO(gpio_virtual,gpios[5],1);
		valueGPIO(gpio_virtual,gpios[6],1);
	}
	else if(num == 6){
		valueGPIO(gpio_virtual,gpios[0],1);
		valueGPIO(gpio_virtual,gpios[1],0);
		valueGPIO(gpio_virtual,gpios[2],1);
		valueGPIO(gpio_virtual,gpios[3],1);
		valueGPIO(gpio_virtual,gpios[4],1);
		valueGPIO(gpio_virtual,gpios[5],1);
		valueGPIO(gpio_virtual,gpios[6],1);
	}
	else if(num == 7){
		valueGPIO(gpio_virtual,gpios[0],1);
		valueGPIO(gpio_virtual,gpios[1],1);
		valueGPIO(gpio_virtual,gpios[2],1);
		valueGPIO(gpio_virtual,gpios[3],0);
		valueGPIO(gpio_virtual,gpios[4],0);
		valueGPIO(gpio_virtual,gpios[5],0);
		valueGPIO(gpio_virtual,gpios[6],0);
	}
	else if(num == 8){
		valueGPIO(gpio_virtual,gpios[0],1);
		valueGPIO(gpio_virtual,gpios[1],1);
		valueGPIO(gpio_virtual,gpios[2],1);
		valueGPIO(gpio_virtual,gpios[3],1);
		valueGPIO(gpio_virtual,gpios[4],1);
		valueGPIO(gpio_virtual,gpios[5],1);
		valueGPIO(gpio_virtual,gpios[6],1);
	}
	else if(num == 9){
		valueGPIO(gpio_virtual,gpios[0],1);
		valueGPIO(gpio_virtual,gpios[1],1);
		valueGPIO(gpio_virtual,gpios[2],1);
		valueGPIO(gpio_virtual,gpios[3],1);
		valueGPIO(gpio_virtual,gpios[4],0);
		valueGPIO(gpio_virtual,gpios[5],1);
		valueGPIO(gpio_virtual,gpios[6],1);
	}
}

module_init( funcion_inicio );
module_exit( funcion_exit );
