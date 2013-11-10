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

TCategoria* Categoria_new(char* nombre, char* desc);

TCategoria* Categoria_from_id(unsigned int id);

TCategoria* Categoria_all(unsigned int *id_p);

unsigned int Categoria_get_id(TCategoria* this);
const char* Categoria_get_nombre(TCategoria* this);
const char* Categoria_get_descripcion(TCategoria* this);

int Categoria_free(TCategoria* this);

TCategoria* Categoria_del(unsigned int id);

unsigned int* Categorias_buscar(char* t, size_t* len);
#endif
