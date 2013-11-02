#ifndef __ARCHIVO_BLOQUE_H__
#define __ARCHIVO_BLOQUE_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


typedef struct TArchivo TArchivo;

/** Crea Archivo de bloques.
 * Crea archivo de bloqes sin administracion de espacio libre con mapa de bits
 * @param char* path: path del archivo
 * @param size_t size: tama~no del bloque
 * @return TArchivo* puntero a archivo de bloques (malloquea memoria).
 */
TArchivo* Archivo_crear(char *path, size_t size);

/** Crea Archivo de bloques.
 * @param char* path: path del archivo
 * @param char* path_adm: path del archivo de administracion (si es NULL se desactiva la administracion)
 * @param size_t size: tama~no del bloque
 * @return TArchivo* puntero a archivo de bloques (malloquea memoria).
 */
TArchivo* Archivo_crear_adm(char *path, char *path_adm, size_t size);

/** Seekea por bloques (whence como el del fseek).
 * Si se quiere leer el bloque en la posicion, despues del seek se debe hacer un Archivo_bloque_leer. Recordar que despues de leer estaremos parados al final del blouqe, si se quiere escribri el bloque leido (por que se agrego algo), hay que seekear devuellta a la posicion del bloque y despues flushear
 * TODO: no chequea que el bloque este dentro del archivo, es decir, si pones un numero bloque mas grande que el del archivo, el comportamiendo no esta definido.
 * @param TArchivo* this: instancia de archivo de bloques
 * @param unsinged int n: numero de bloque
 * @param int whence: como fseek
 * @return int 0->ok, resto error
 */
int Archivo_bloque_seek(TArchivo* this, unsigned int n, int whence);

/* Destruye el bloque que esta actualmente en memoria y crea uno vacio en memoria.
 * Para escribir hay que flushear. Sirve para cuando seekeas queres reemplazar el bloque existente.
 * @param TArchivo* this: instancia de archivo de bloques
 * @return int 0->ok, resto error
 */
int Archivo_bloque_new(TArchivo* this);

/** Lee bloque del archivo.
 * Va a correr el fd al final del bloque, para escribir el bloque leido a disco, hay que seekear al ppcio del bloque y despues flushear.
 * @param TArchivo* this: instancia de archivo de bloques
 * @return int 0->ok, resto error
 */
int Archivo_bloque_leer(TArchivo* this);

/** Devuelve buffer del bloque abierto, de finalizar el bloque lee el bloque siguiente y devuelve el buffer.
 * Sirve para leer secuencialmente todo el archivo. Si no se qiere hacer eso, usar el metodo Archivo_get_bloque_buf.
 * @param TArchivo* this: instancia de archivo de bloques
 * @param size_t size: tama~no del buffer (parametro de salida)
 * @return uint8_t*: puntero a buffer malloqueado (El usuario debe liberar!) o NULL si error
 */
uint8_t* Archivo_get_buf(TArchivo* this, size_t* size);

/** Devuelve buffer del bloque abierto. Se considera error que ya se hallan leido todos los registros del bloque.
 * * @param TArchivo* this: instancia de archivo de bloques
 * @param size_t size: tama~no del buffer (parametro de salida)
 * @return uint8_t*: puntero a buffer malloqueado (El usuario debe liberar!) o NULL si error
 */
uint8_t* Archivo_get_bloque_buf(TArchivo* this, size_t* size);

/** Agrega(escribe) un buffer de informacion al archivo. De llenarse el bloque actual, lo escribe a disco y crea uno nuevo.
 * Sirve para agregar secuencialmente registros, si se qiere tener un manejo de bloques no usar este metodo, usar Archivo_bloque_agregar_buf.
 * @param TArchivo* this: instancia de archivo de bloques
 * @param uint8_t* buff: buffer que se escribe en el bloque (no se modifica)
 * @param size_t size: tama~no del buffer
 * @return int 0-> ok, resto error
 */
int Archivo_agregar_buf(TArchivo* this, uint8_t* buff, size_t size);

/** Devuelve si el bloque corriente tiene espacio libre para escribir.
 * Hace una llamada Bloque_libre pasandole la instancia de bloque corriente.
 * @param TArchivo* this: instancia del archivo
 * @param size_t size: tama~no de lo que se quiere escribir
 * @return int 1-> tiene, 0-> no tiene o error error
 */
int Archivo_bloque_libre(TArchivo* this, size_t size);

/** Devuelve si el bloque n esta lleno (utilizando la logica del mapa de bits)
 * @param TArchivo* this: instancia del archivo
 * @param size_t n_bloque: numero de bloque, numerado desde 0
 * @return int numero -> lleno, 0-> no lleno
 */
int Archivo_libre(TArchivo* this, size_t n_bloque);

/** Agrega(escribe) un buffer de informacion al bloque corriente.
 * Hace una llamada a Bloque_agregar_buf pasandole la instancia de bloque corriente.
 * @param TArchivo* this: instancia de archivo.
 * @param uint8_t* buff: buffer que se escribe en el bloque (no se modifica)
 * @param size_t size: tama~no del buffer
 * @return int 0-> ok, resto error
 */
int Archivo_bloque_agregar_buf(TArchivo* this, uint8_t* buff, size_t size);

/** Fuerza la escritura del bloque actual a disco.
 * NOTA: lo escribe donde este parado actualmente el fd, recordar que si se leyo el bloque, el fd va a estar parado al final del bloque en disco, habra que seekear al ppcio para dsp escribir.
 * @param TArchivo* this: instancia de archivo de bloques
 * @return int 0-> ok, resto error
 */
int Archivo_flush(TArchivo* this);

/** Cierra Archivo.
 * Por si se nececita hacer fclose en el fp, se corserva la posicion del archivo para reabrirlo posteriormente.
 * @param TArchivo* this: instancia de archivo de bloques
 * @return int 0->ok, resto error.
 */
int Archivo_close(TArchivo* this);

/** Abre Archivo.
 * Abre el archivo en la posicion guardada cuando se hizo el Archivo_close.
 * @param TArchivo* this: instancia de archivo de bloques
 * @return int 0->ok, resto error.
 */
int Archivo_open(TArchivo* this);

/** Destruye Archivo.
 * @param TArchivo* this: instancia de archivo de bloques
 * @return int 0->ok, resto error.
 */
int Archivo_destruir(TArchivo* this);

#endif
