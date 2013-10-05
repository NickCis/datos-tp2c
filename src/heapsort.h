#ifndef __HEAPSORT_H__
#define __HEAPSORT_H__
#include <stdlib.h>

/** Tipo de funcion usada para comparar dos elementos. (De esta manera ordena de mayor a menor)
 * @param void* puntero 1
 * @param void* puntero 2
 * @return int: -1 puntero 1 es mayor, 0 son iguales, 1 punter 2 es mayor
 */
typedef int (*TCompareFunction)(void*, void*);

void heapsort(void** array, size_t size, TCompareFunction cmp);

#endif
