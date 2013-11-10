#ifndef __LIBRES_BLOQUES_H__
#define __LIBRES_BLOQUES_H__
#include <stdlib.h>

/** Lee archivo de bloques libres
 */
int Libres_read(char* path, size_t *len, unsigned int** arr);

/** Escribe archivo de bloques libres
 */
int Libres_write(char* path, size_t len, unsigned int* arr);

/**Agrega numero de bloque a bloques libres.
 */
int Libre_agregar(size_t* len, unsigned int **arr, unsigned int l);

/** Devuelve verdadero si hay un bloque libre, lo saca de la lsita y lo devuelve en l
 * @param l[out]: numero de bloque libre;
 * @return 0 -> no hay, 1 si hay
 */
int Libre_pop(size_t* len, unsigned int *arr, unsigned int* l);

#endif
