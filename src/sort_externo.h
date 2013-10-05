#ifndef __SORT_EXTERNO_H__
#define __SORT_EXTERNO_H__

/** Tipo de funcion usada para comparar dos buffers.
 * @param void* buffer 1
 * @param size_t tama~no del buffer 1
 * @param void* buffer 2
 * @param size_t tama~no del buffer 2
 * @return int: -1 buffer 1 es mayor, 0 son iguales, 1 buffer 2 es mayor
 */
typedef int (*TSortExternFunction)(void*, size_t, void*, size_t);


/** Realiza division en archivos ordenados por el metodo de replacement selection.
 * @param char* in_file: path del archivo de bloques de entrada.
 * @param char* tmp_name: formato de sprintf para path archivos de salida (maximo 254 caracteres) (tiene que tener un %d qe indicara el numero de archivo.
 * @param size_t block_size: tama~no de bloque de los archivos.
 * @param size_t array_size: tama~no del array que se usara para el ordenamiento interno.
 * @param TSortExterFunction cmp: funcion de comparacion.
 */
int replacememt_selection(char* in_file, char* tmp_name, size_t block_size, size_t array_size, TSortExternFunction cmp);

/** Hace merge de varios archivos de bloques ordenados.
 * Para mergear se lee el primer bloque de N archivos, se cierran los archivos. Se ordenan de acuerdo al primer registro de cada bloque, se saca el menor y se escribe a un archivo, reemplazandolo en el vector por el siguiente registro del bloque. Si no hay siguiente registro en el bloque, se abre ese archivo, se lee le siguiente bloque y se lo vuelve a cerrar. Se repetie el proceso hasta que se hallan mergeado todos los registros de todos los bloques. Si N es menor qe la cantidad de archivos, se llama a la misma funcion con los archivos faltantes mas el archivo producto del mergeo hasta que el resultado sea un solo archivo.
 *
 * @param char* out_path: path archivo final de salida.
 * @param char* tmp_name: formato de sprintf para path archivos de entrada (maximo 254 caracteres) (tiene que tener un %d qe indicara el numero de archivo.
 * @param size_t block_size: tama~no de bloque de los archivos.
 * @param size_t array_size: Cantidad de bloques que se mantienen en memoria
 * @param size_t desde: numero de archivo inicial para el mergeo.
 * @param size_t desde: numero de archivo final+1 para el mergeo (ej: ultimo archivo es nro 7, se le pasa un 7)
 * @param TSortExterFunction cmp: funcion de comparacion.
 */
int merge_externo(char* out_path, char* tmp_name, size_t block_size, size_t array_size, size_t desde, size_t hasta, TSortExternFunction cmp);

/** Realiza un sort externo en el archivo de bloques in_file poniendo la salida en el archivo de bloques out_file usando la funcion de comparacion cmp.
 * Internamente llama a replacement_selection con valores default para tmp_name y array_size, y dsp hace el merge.
 *
 * @param size_t: tama~no de bloque de los archivos
 * @param char* in_file: path del archivo de bloques de entrada
 * @param char* out_file: path del archivo de bloques de salida
 * @param TSortFunction cmp: puntero a funcion usada para comparar
 * @return 0: ok, resto error.
 */
int sort_externo(size_t size, char* in_file, char* out_file, TSortExternFunction cmp);
#endif
