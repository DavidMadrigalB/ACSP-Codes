/*
 *  ddc-tf.c - Asignación de numero mayor y numero menor,
 *	(se usa asignación dinámica).
 *	Se utiliza la creación de clase y archivo de dispositivo
 *	(se usa creación automática).
 *	Se colocan las operaciones de archivo.
 *	Se coloca la definición de las operaciones de archivo
 *	Para un device driver de caracter.
 */
#include <linux/module.h>		/* Necesaria para todos los modulos 			*/
#include <linux/kernel.h>		/* Necesaria para KERN_INFO 				*/
#include <linux/init.h>			/* Necesaria para las macros 				*/
#include <linux/fs.h>			/* Necesaria para las funciones de registro		*/
#include <linux/kdev_t.h>		/* Necesaria para las macros 		MAJOR,MINOR	*/
#include <linux/device.h>		/* Necesaria para el dispositivo			*/
#include <linux/slab.h>			/* Necesaria para el kmalloc y kfree			*/
#include <linux/cdev.h>			/* Necesaria para cdev					*/
#include <linux/moduleparam.h>		/* Necesaria para las macros de parametros 		*/
#include <linux/sched.h>
#include <linux/delay.h>
#include <asm/io.h>
#define ADDR_BASE_GPIO 0xfe200000
#define MEM_SIZE 1024

MODULE_LICENSE("GPL");				/* Tipo de licencia				*/
MODULE_AUTHOR("VICTOR H GARCIA O");		/* Autor del módulo 				*/
MODULE_DESCRIPTION("Manejo de macro __init");	/* Descripción de la funcionalidad del módulo 	*/
MODULE_VERSION("1.0");				/* Versión del módulo 				*/
MODULE_INFO(driver, "PULSO CARDIACO"); 		/* Información personalizada del usuario	*/

void configGPIO( volatile uint32_t *gpio_virtual, int gpio, int modo );
void valueGPIO( volatile uint32_t *gpio_virtual, int gpio, int valor );
void dibuja_numero( int num );

struct task_struct *khilo;
volatile uint32_t *gpio_virtual;
int gpios[7] = { 14, 15, 18, 23, 24, 25, 8 };

static dev_t dispositivo;		/* Debe de ser un numero que no este en cat/proc/devices */
static struct class * dev_class;
static struct device * dev_file;
static struct cdev dev_cdev;
static char *buffer;

static int driver_open		( struct inode *inode, struct file *file );
static int driver_release	( struct inode *inode, struct file *file );
static ssize_t driver_read	( struct file *filp, char __user *buf, size_t len, loff_t * off );
static ssize_t driver_write	( struct file *filp, const char *buf, size_t len, loff_t * off );

static struct file_operations fops =
{
	.owner   = THIS_MODULE,
	.read    = driver_read,
	.write   = driver_write,
	.open    = driver_open,
	.release = driver_release,
};

static int driver_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "---------------------------------------------------------\n");
	printk(KERN_INFO "Ejecutando la operacion de open \n");

	buffer = (char *)kmalloc(MEM_SIZE, GFP_KERNEL);
	if(buffer == NULL)
	{
		printk(KERN_ERR "Error al asignar memoria \n");
		return -ENOMEM;
	}

	return 0;
}

static int driver_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "---------------------------------------------------------\n");
	printk(KERN_INFO "Ejecutando la operacion de release \n");

	kfree(buffer);

	return 0;
}

static ssize_t driver_read(struct file *filp, char __user *buf, size_t len, loff_t * off)
{

	int ret,lon;

	printk(KERN_INFO "---------------------------------------------------------\n");
	printk(KERN_INFO "Ejecutando la operacion de read \n");

	printk(KERN_INFO "Len: %ld, offset: %lld \n", len, *off);

	if( *off == 0 && len > 0)
	{
		strcpy(buffer, "Hola!!!");
		lon = strlen( buffer ) + 1;
		ret = 	copy_to_user(buf, buffer, lon);
		if( ret )
		{
			printk(KERN_ERR "Error al copiar el buffer a espacio de usuario \n");
			return -EFAULT;
		}
		(*off) += lon;
		return lon;
	}

	return 0;

}

static ssize_t driver_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{

	int ret;
	register int j;

	printk(KERN_INFO "---------------------------------------------------------\n");
	printk(KERN_INFO "Ejecutando la operacion de write \n");

	printk(KERN_INFO "Len: %ld, offset: %lld \n", len, *off);

	ret = copy_from_user( buffer, buf , len);
	if( ret )
	{
		printk(KERN_ERR "Error al copiar desde espacio de usuario \n");
		return -EFAULT;
	}
	printk(KERN_INFO "Buffer recibido: %s \n", buffer);
	j=0;
	while(buffer[j]!='\0'){
		dibuja_numero(buffer[j]);
       		ssleep(1);
		j++;
	}
	return len;

}

static int __init funcion_inicio(void)
{
	int ret;
	register int i;
	printk(KERN_INFO "---------------------------------------------------------\n");
	printk(KERN_INFO "Iniciando el DDC \n");
	printk(KERN_INFO "Registrando el divice driver de caracter... \n");

	ret = alloc_chrdev_region(&dispositivo, 0, 1, "ESCOM_DDC");

	if(ret < 0)
	{
		printk(KERN_ERR "Error al registrar el device driver de caracter \n");
		return ret;
	}

	printk(KERN_INFO "Dispositivo registrado Exitosamente... \n");
	printk(KERN_INFO "Numero mayor asignado: %d \n",MAJOR(dispositivo));
	printk(KERN_INFO "Numero menor asignado: %d \n",MINOR(dispositivo));

	cdev_init(&dev_cdev, &fops);
	ret = cdev_add( &dev_cdev, dispositivo, 1);
	if(ret < 0)
	{
		printk(KERN_ERR "Error al registrar las operaciones del device driver de caracter \n");
		unregister_chrdev_region(dispositivo, 1);
		return ret;
	}
	printk(KERN_INFO "Operaciones registradas Exitosamente... \n");

	dev_class = class_create(THIS_MODULE,"ESCOM_class");
	if(IS_ERR(dev_class))
	{
		printk(KERN_ERR "Error al crear la clase del dispositivo \n");
		cdev_del(&dev_cdev);
		unregister_chrdev_region(dispositivo, 1);
		return PTR_ERR(dev_class);
	}
	printk(KERN_INFO "Clase Creada Exitosamente... \n");
	dev_file = device_create(dev_class, NULL, dispositivo, NULL, "ESCOM_device");
	if(IS_ERR(dev_file))
	{
		printk(KERN_ERR "Error al crear el dispositivo \n");
		class_destroy(dev_class);
		cdev_del(&dev_cdev);
		unregister_chrdev_region(dispositivo, 1);
		return PTR_ERR(dev_file);
	}
	printk(KERN_INFO "Dispositivo Creado Exitosamente... \n");
	gpio_virtual = (uint32_t *)ioremap( ADDR_BASE_GPIO, 0X30 );

	for(i=0;i<7;i++){
		configGPIO(gpio_virtual,gpios[i],0);
	}

	return 0;
}

static void __exit funcion_exit(void)
{
	printk(KERN_INFO "---------------------------------------------------------\n");
	printk(KERN_INFO "Terminando la ejecucion del DDC... \n");

	device_destroy(dev_class,dispositivo);
	class_destroy(dev_class);
	cdev_del(&dev_cdev);
	unregister_chrdev_region(dispositivo, 1);
	iounmap( gpio_virtual );
}


void valueGPIO( volatile uint32_t *gpio_virtual, int gpio, int valor ){
	int bit = (gpio >> 5); //gpio / 32
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

void dibuja_numero(int num){
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


module_init(funcion_inicio);
module_exit(funcion_exit);
