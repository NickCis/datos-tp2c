#ifndef __RTT_H__
#define __RTT_H__
#include <stdint.h>

typedef struct TRtt TRtt;

/** Crea una instancia de Rtt.
 * @param path_ocu_apa[in]: path al archivo de ocurrencias por aparicion
 * @param path_ocu_doc[in]: path del archivo de (idT, idDoc)
 * @param path_arb[in]: path del archivo para el arbol
 * @param path_lista[in]: archivo para la lista invertida
 * @param path_lista_baja[in]: archivo para lista invertiad
 * @param orden_arb[in]: orden del arbol
 * @param block_lista[in]: tama~no de bloque que usa la lista
 * @return Instancia de rtt.
 */
TRtt* Rtt_crear(char* path_ocu_apa, char* path_ocu_doc, char* path_arb, char* path_lista, char* path_lista_baja, size_t orden_arb, size_t block_lista);

/** Agrega texto. Se debe usar el generar indice dsp.
 * @param this[in]: instancia de rtt
 * @param id_text[in]: id del texto
 * @param texto[in]: texto.
 * @return 0->ok, resto error
 */
int Rtt_agregar_texto(TRtt* this, unsigned int id_text, char* texto);

/** Busca un termino, devuelve lista de ids de documentos.
 * XXX: lo que devuelve deberia ser unsigned int*, no long*.
 * @param this[in]: instancia de rtt
 * @param t[in]: termino
 * @param len[out]: largo de la lista
 * @return lista
 */
long* Rtt_buscar(TRtt* this, char* t, size_t* len);

/** Remueve termino
 * @param this[in]: instancia de rtt
 * @param t[in]: termino
 * @return 0->ok, resto error
 */
int Rtt_remover_termino(TRtt* this, char* t);

/** Genera indice.
 * @param this[in]: instancia de rtt
 * @return 0->ok, resto error
 */
int Rtt_generar_indice(TRtt* this);

/** Destruye instncia de rtt.
 * @param this[in]: instancia de rtt
 * @return 0->ok, resto error
 */
int Rtt_destruir(TRtt* this);


#endif
