#ifndef __CATEGORIAS_H__
#define __CATEGORIAS_H__
#include <stdlib.h>
typedef struct TCategoria TCategoria;

/** Inicializa uso de categorias
 */
int Categorias_init();

/** Finaliza uso de categorias
 */
int Categorias_end();

/** Crea una nueva categoria.
 * @param nombre
 * @param desc
 * @return TDA categoria.
 */
TCategoria* Categoria_new(char* nombre, char* desc);

/** Trae una categoria buscandola desde la id.
 * @param id
 * @return TDA categoria
 */
TCategoria* Categoria_from_id(unsigned int id);

/** Itera sobre todas las caterias, devolviendo una en cada iteracion.
 * Inicialmente el usuario debe setear el valor del entero id_p en 0, y dsp, uno no tiene que modificar dicho valor, este sera modificado internamiente por la funcion.
 * @param id_p puntero a un entero que inicialmente uno tiene que setear en 0
 * @return TDA de categoria o NULL si no hay mas.
 */
TCategoria* Categoria_all(unsigned int *id_p);

/** Devuelve el id de la categoria.
 * @param this categoria
 * @return id
 */
unsigned int Categoria_get_id(TCategoria* this);
/** Devuelve el nombre de la categoria
 * @param this categoria
 * @return nombre
 */
const char* Categoria_get_nombre(TCategoria* this);
/** Devuelve la descripccion de la categoria.
 * @param this categoria
 * @return descripccion
 */
const char* Categoria_get_descripcion(TCategoria* this);

/** Libera tda de categoria.
 * @param this categoria
 * @return 0 ok, resto error
 */
int Categoria_free(TCategoria* this);

/** Borra categoria
 * @param id
 * @return categoria borrada
 */
TCategoria* Categoria_del(unsigned int id);

/** Busca utilizando RTT la categoria.
 * @param t descripcion
 * @param len[out] largo de la lista
 * @return lista de ids
 */
unsigned int* Categorias_buscar(char* t, size_t* len);
#endif
