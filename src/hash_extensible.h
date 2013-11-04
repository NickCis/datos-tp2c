#ifndef __HASH_EXTENSIBLE_H__
#define __HASH_EXTENSIBLE_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct THashExtensible THashExtensible;

/** Tipo de funcion para dispercion
 * @param id: id del elemento
 * @param tam_tabla: tama~no de la tabla
 * @return numero de bloque
 */
typedef unsigned int(*TDispersionFunct)(unsigned int id, unsigned int tam_tabla);

/** Funcion que recibe un elemento y devuelve el id del mismo
 * @param ele: buffer del elemento
 * @param size: tama~no del buffer
 * @return id del elemento
 */
typedef unsigned int(*THashId)(uint8_t* ele, size_t size);

/** Crea un hash extensible
 * @param path: ruta del archivo donde se guardaran los bloques
 * @param path_tabla: ruta del archivo donde se guardara la tabla
 * @param path_baja: ruta del archivo donde se guardaran los bloques libres
 * @param block_size: tama~no de bloque a usar
 * @param funct: funcion de dispersion a usar
 * @param get_id: funcion para obtener el id del elmento
 * @return instancia de hash extensible o NULL si error
 */
THashExtensible* HashExtensible_crear(char* path, char* path_tabla, char* path_baja, size_t block_size, TDispersionFunct funct, THashId get_id);

/** Inserta un elemento en el hash.
 * Si ya existe un elemento con el id, se sale con error.
 * @param this: instancia del HashExtensible
 * @param ele: elemento a insertar
 * @param size: tama~no del elemento
 * @return 0 -> ok, resto error
 */
int HashExtensible_insertar(THashExtensible* this, uint8_t* ele, size_t size);

/** Obtiene un elemento del hash.
 * Hay que liberar el puntero que devuelve!.
 * @param this: instancia del HashExtensible
 * @param id: id del elemento
 * @param size[out]: tama~no del elemento
 * @return elemento o NULL si no existe.
 */
uint8_t* HashExtensible_get(THashExtensible* this, unsigned int id, size_t *size);

/** Elimina el elemento del hash.
 * @param this: instancia del HashExtensible
 * @param id[in]: id del elemento
 * @param size[out]: tama~no del elemento que se elimino
 * @return elemento que se removio del hash  o NULL si error.
 */
uint8_t* HashExtensible_del(THashExtensible* this, unsigned int id, size_t *size);

/** Destruye el hash.
 * @param this: instancia del HashExtensible
 * @return 0 ->ok, resto error
 */
int HashExtensible_destruir(THashExtensible* this);

/** Funcion modulo para usar como funcion de dispersion
 */
unsigned int HashDispersionModulo(unsigned int id, unsigned int tam_tabla);


#endif
