datos-tp2c
==========

Compilar:
$ cd src
$ make clean all

Particularmente esto generara dos ejecutables:
* creador\_admin
* main

creador\_admin: sirve para crear al super adminitrador (Tiene un menu que bastante didactico)
main: el es programa, tambien tiene un menu bastante didactico

Hay una serie de "tests" (son mas ejemplos de como usar los distintos modulos)

$ cd src
$ make clean test

Se generara una carpeta test (dento de src), con todos los ejecutables. Para saber como usarlos, se puede revisar el codigo de cada uno, estan todos en la carpeta test.

Se usa un sistema de configuracion en tiempo de compilacion, todos los parametros son configurables cambiando el archivo config.h.
