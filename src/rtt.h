#ifndef __RTT_H__
#define __RTT_H__
#include <stdint.h>

typedef struct TRtt TRtt;

TRtt* Rtt_crear(char* path_ocu_apa, char* path_ocu_doc, char* path_arb, char* path_lista, char* path_lista_baja, size_t orden_arb, size_t block_lista);
int Rtt_agregar_texto(TRtt* this, unsigned int id_text, char* texto);

long* Rtt_buscar(TRtt* this, char* t, size_t* len);
//int Rtt_remover_termino(TRtt* this, char* t);

int Rtt_generar_indice(TRtt* this);

int Rtt_destruir(TRtt* this);


#endif
