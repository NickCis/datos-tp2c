#ifndef __ARBOlBMAS_H__
#define __ARBOlBMAS_H__

#include <stdint.h>

typedef struct TArbolBM TArbolBM;

/** Crea el arbol b+.
 * @param path: path del archivo que se usara para guardar
 * @param orden: orden del arbol
 * @return nueva instancia de Arbol
 */
TArbolBM* Arbol_crear(char *path, size_t orden);

/** Destruye la instancia de arbol.
 * @param this:instancia del arbol
 */
void Arbol_destruir(TArbolBM* this);

/** Inserta elmento en arbol.
 * @param this:instancia del arbol
 * @param id
 * @param ptr: elemento a guardar
 * @return 0->ok, resto->error
 */
int Arbol_insertar(TArbolBM* this, long id, long ptr);

/** Obtiene un elemento del arbol.
 * @param this:instancia del arbol
 * @param id
 * @param ptr: elemento
 * @return 0->ok, resto error
 */
int Arbol_get(TArbolBM* this, long id, long* ptr);

/** Remueve elemento.
 * @param this:instancia del arbol
 * @param id
 * @return 0->ok, resto error
 */
int Arbol_remover(TArbolBM* this, long id);



#endif
