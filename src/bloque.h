#ifndef __BLOQUE_H__
#define __BLOQUE_H__
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define BLOQUE_PORCENTAGE 70

typedef struct TBloque TBloque;

/** Crea bloque. De registros de longitud variable.
 * @param size_t size: tama~no del bloque
 * @return TBloque* puntero a bloque (malloquea memoria).
 */
TBloque* Bloque_crear(size_t size);

/** Crea bloque de registros de longitud fija.
 * @param size_t size: tama~no del bloque
 * @param size_t size_reg: tama~no del registro
 * @return TBloque* puntero a bloque (malloquea memoria).
 */
TBloque* BloqueFijo_crear(size_t size, size_t size_reg);

/** Lee Bloque.
 * @param TBloque* this: instancia de bloque
 * @param FILE* fd: file descriptor de archivo para leer el bloque.
 * @return int 0-> ok, resto error
 */
int Bloque_leer(TBloque* this, FILE* fd);

/** Escribe bloque a archivo.
 * @param TBloque* this: instancia de bloque
 * @param FILE* fd: file descriptor de archivo para escribir el bloque.
 * @return int 0-> ok, resto error
 */
int Bloque_escribir(TBloque* this, FILE* fd);

/** Agrega(escribe) un buffer de informacion al bloque.
 * Solo para bloques de registros de longitud variable.
 * @param TBloque* this: instancia de bloque
 * @param uint8_t* buff: buffer que se escribe en el bloque (no se modifica)
 * @param size_t size: tama~no del buffer
 * @return int 0-> ok, resto error
 */
int Bloque_agregar_buf(TBloque* this, uint8_t* buff, size_t size);

/** Agrega(escribe) un buffer de informacion al bloque.
 * Solo para bloques de registros de longitud fija.
 * @param TBloque* this: instancia de bloque
 * @param uint8_t* buff: buffer que se escribe en el bloque (no se modifica)
 * @return int 0-> ok, resto error
 */
int BloqueFijo_agregar_buf(TBloque* this, uint8_t* buff);

/** Devuelve si el bloque tiene espacio libre para escribir.
 * Solo para bloques de resgistros longitud variable.
 * @param TBloque* this: instancia de bloque
 * @param size_t size: tama~no de lo que se quiere escribir
 * @return 0-> tiene, otro no tiene o  error error
 */
int Bloque_libre(TBloque* this, size_t size);

/** Devuelve si el bloque tiene espacio libre para escribir.
 * Solo para bloques de registros longitud fija.
 * @param TBloque* this: instancia de bloque
 * @return 0-> tiene, otro no tiene o  error error
 */
int BloqueFijo_libre(TBloque* this);

/** Devuelve si el bloque esta lleno teniendo en cuenta el porcentage establecido.
 * Es lo que usa el archivo para saber si marcar en 1 el bit del mapa para el control de espacio libre.
 * @param TBloque* this: instancia de bloque
 * @return int 1-> lleno, 0-> vacio
 */
int Bloque_lleno(TBloque* this);

/** Devuelve el Buff (registro) numero n que este en el buffer, HAY QE LIBERAR EL PUNTERO QE DEVUELVE!!.
 * @param TBloque* this: instancia de bloque
 * @param int n: numero de registro que se quiere leer.
 * @param size_t size: tama~no del buffer (parametro de salida)
 * @return uint8_t*: puntero a buffer malloqueado (El usuario debe liberar!) o NULL si error
 */
uint8_t* Bloque_get_buf(TBloque* this, int n, size_t* size);

/** Destruye bloque.
 * @param TBloque* this: instancia de bloque
 * @return int 0->ok, resto error.
 */
int Bloque_destruir(TBloque* this);
#endif
