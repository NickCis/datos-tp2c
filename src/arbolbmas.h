#ifndef __ARBOlBMAS_H__
#define __ARBOlBMAS_H__


typedef struct TArbolBM TArbolBM;

TArbolBM* Arbol_crear(char *path);
/* open or create index file, find free entry in index table,
    fill in entry, DupKeys is 1 if duplicate keys are allowed, 0 if not,
    return number of entry in table */

void Arbol_destruir(TArbolBM* this);
/* close Index file, free entry in index info table */

int Arbol_insertar(TArbolBM* this, long Key, long Ptr);
/* insert Key & Ptr in index file, return 0 for success, nonzero for error */

long Arbol_get(TArbolBM* this, long Key);
/* return pointer for entry in Index file <= Key, return -1 if none */



#endif
