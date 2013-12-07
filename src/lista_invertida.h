#ifndef __LISTA_INVERTIDA_H__
#define __LISTA_INVERTIDA_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct TListaInvertida TListaInvertida;

/** Crea una lista Invertida
 * @param path: ruta del archivo donde se guardaran los bloques
 * @param path_baja: ruta del archivo donde se guardaran los bloques libres
 * @param block_size: tama~no de bloque a usar
 * @return instancia de la lista  o NULL si error
 */
TListaInvertida* ListaInvertida_crear(char* path, char* path_baja, size_t block_size);

/** Crea un elemento de la lista invertida.
 * @param this: instancia del ListaInvertida
 * @return referencia a la lista.
 */
unsigned int ListaInvertida_new(TListaInvertida* this);

/** Borra el elemento corriente
 * @param this: instancia del ListaInvertida
 */
int ListaInvertida_erase(TListaInvertida* this);

/** Setea el elemento corriente
 * @param this: instancia del ListaInvertida
 * @return 0 ok, resto error
 */
int ListaInvertida_set(TListaInvertida* this, unsigned int ref);

/** Obtiene elemento de la lista.
 * Se devolveran los elementos por orden el que se hallan guardado.
 * @param this: instancia del ListaInvertida
 * @param size[out] tama~no del buffer.
 * @return puntero al buffer, o NULL si error.
 */
uint8_t* ListaInvertida_get(TListaInvertida* this, size_t* size);

/** Agrega un elemento a la lista corriente.
 * @param this: instancia del ListaInvertida
 * @param ele elemento.
 * @param size tama~no del buffer.
 * @return 0 ok, resto error
 */
int ListaInvertida_agregar(TListaInvertida* this, uint8_t* ele, size_t size);

/** Escribe a disco.
 * @param this: instancia del ListaInvertida
 * @return 0 ok, resto error
 */
int ListaInvertida_escribir(TListaInvertida* this);

/** Crea una lista Invertida
 * @param this: instancia del ListaInvertida
 * @return 0 ok, resto error
 */
int ListaInvertida_destruir(TListaInvertida* this);

#endif
