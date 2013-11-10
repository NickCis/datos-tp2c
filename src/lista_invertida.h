#ifndef __LISTA_INVERTIDA_H__
#define __LISTA_INVERTIDA_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct TListaInvertida TListaInvertida;

TListaInvertida* ListaInvertida_crear(char* path, char* path_baja, size_t block_size);

/** Crea un elemento de la lista invertida.
 * Devuelve su referencia
 */
unsigned int ListaInvertida_new(TListaInvertida* this);

/** Borra el elemento corriente
 */
int ListaInvertida_erase(TListaInvertida* this);

/** Setea el elemento corriente
 */
int ListaInvertida_set(TListaInvertida* this, unsigned int ref);

uint8_t* ListaInvertida_get(TListaInvertida* this, size_t* size);

int ListaInvertida_agregar(TListaInvertida* this, uint8_t* ele, size_t size);
int ListaInvertida_escribir(TListaInvertida* this);

int ListaInvertida_destruir(TListaInvertida* this);

#endif
