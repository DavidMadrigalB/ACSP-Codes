# ACSP-Codes
Códigos sobre la optativa de Advanced Computing Systems Programming (ACSP), cursada en ESCOM, IPN. Por el profesor Victor Hugo García Ortega.

Compilando módulos de kernel: kbuild

*Nota: Todos los comandos son para **Linux***.

## Para generar los módulos (arhivos .ko):
En el Makefile, debes poner el nombre del archivo .c (Que contiene el código del módulo del kernel)

En el directorio escribir:

**make**

Para limpiar los archivos generados, escribir:

**make clean**

## Cómandos de los módulos:
Generalmente estos comandos necesitan de superusuario, así que van seguidos de **sudo**

**insmod**    - Insertar módulo al kernel de Linux

**rmmod**     - Remueve módulo del kernel de Linux

**ismod**     - Muestra los módulos cargados en el kernel de Linux. Se encuentran almacenados en /proc/modules

**modinfo**   - Proporciona información del módulo

Ejemplo:

insmod file.ko

## Cómandos para ver el módulo ya insertado:
**lsmod**                     - Muestra todos los módulos activos

**lsmod | grep name**         - Filtra en los módulos activos un nombre

**dmesg**                     - Muestra el contenido de syslog

**tail -f /var/log/syslog**   - Muestra las últimas 10 líneas del contenido de syslog
