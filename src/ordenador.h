#ifndef __ORDENADOR_H__
#define __ORDENADOR_H__
#include <stdlib.h>
//#include <stdint.h>

/** Tipo de funcion usada para comparar dos buffers.
 * @param void* puntero 1
 * @param void* puntero 2
 * @return int: -1 puntero 1 es mayor, 0 son iguales, 1 punter 2 es mayor
 */
typedef int (*TSortFunction)(void*, void*);
typedef struct TOrdenador TOrdenador;

/** Crea instancia de ordenador.
 * La funcion para comparar devuelve -1 si el primero es mayor, 0 si son iguales, y 1 si el segundo es mayor.
 * @param TSortFunction funcion para comparar.
 * @return TOrdenador*: instancia de Ordenados
 */
TOrdenador* Ordenador_crear(TSortFunction func);

/** Agrega elemento a Ordenador.
 * @param TOrdenador* this: instancia de ordenador.
 * @param void* puntero a elemento.
 * @return int 0: ok, resto error
 */
int Ordenador_agregar(TOrdenador* this, void *buf);

/** Saca elemento del ordenador (Siempre los saca en orden).
 * @param TOrdenador* this: instancia de ordenador.
 * @return void* puntero a elemento.
 */
void* Ordenador_quitar(TOrdenador* this);

/** Destruye ordenador
 * @param TOrdenador* this: instancia de ordenador.
 * @return int 0: ok, resto error
 */
int Ordenador_destruir(TOrdenador* this);

#endif
