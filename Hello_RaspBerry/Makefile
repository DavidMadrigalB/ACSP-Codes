obj-m += hello-1.o
obj-m += hello-2.o
obj-m += hello-3.o
obj-m += hello-4.o

all:
#	Para compilar el módulo de kernel en PC con Ubuntu:
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

#	Para compilar el módulo de kernel con el compilador cruzado para la Raspberry PI 3:
#	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C /home/victorhgo/linux M=$(PWD) modules
#	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C /home/$(USER)/linux M=$(PWD) modules
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C ~/linux M=$(PWD) modules

clean:
#	Para compilar el módulo de kernel en PC con Ubuntu:
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

#	Para compilar el módulo de kernel con el compilador cruzado para la Raspberry PI 3:
#	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C /home/victorhgo/linux M=$(PWD) clean
#	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C /home/$(USER)/linux M=$(PWD) clean
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C ~/linux M=$(PWD) clean

compilar:
	arm-linux-gnueabihf-gcc hola.c -o hola
