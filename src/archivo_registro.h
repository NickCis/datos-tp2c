#ifndef __ARCHIVO_REGISTRO_H__
#define __ARCHIVO_REGISTRO_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct TArchivoReg TArchivoReg;

/** Crea Archivo de registros.
 * @param char* path: path del archivo
 * @return TArchivoReg* puntero a archivo de registros (malloquea memoria).
 */
TArchivoReg* ArchivoReg_crear(char *path);

/** Lee registro y lo devuelve.
 * @param TArchivoReg* this: instancia de archivo de registro
 * @param size_t size: tama~no del buffer (parametro de salida)
 * @return uint8_t*: puntero a buffer malloqueado (El usuario debe liberar!) o NULL si error
 */
uint8_t* ArchivoReg_leer(TArchivoReg* this, size_t* size);

/** Escribe buffer(registro) en el archivo. 
 * @param TArchivoReg* this: instancia de archivo de registro
 * @param uint8_t* buff: buffer que se escribe en el archivo (no se modifica)
 * @param size_t size: tama~no del buffer
 * @return int 0-> ok, resto error
 */
int ArchivoReg_escribir(TArchivoReg* this, uint8_t* buff, size_t size);

/** Destruye Archivo.
 * @param TArchivoReg* this: instancia de archivo de registro
 * @return int 0-> ok, resto error.
 */
int ArchivoReg_destruir(TArchivoReg* this);

/** Seekea el final del archivo.
 */
int ArchivoReg_seek_end(TArchivoReg* this);
int ArchivoReg_seek_start(TArchivoReg* this);

#endif
