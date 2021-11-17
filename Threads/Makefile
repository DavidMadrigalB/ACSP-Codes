obj-m += thread-1.o
obj-m += thread-2.o
obj-m += thread-3.o
obj-m += thread-4.o

all:
#	Para compilar el módulo de kernel en PC con Ubuntu:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
#	Para compilar el módulo de kernel
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
