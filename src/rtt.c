#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "rtt.h"
#include "arbolbmas.h"
#include "lista_invertida.h"
#include "archivo_registro.h"
#include "archivo_bloque.h"
#include "sort_externo.h"

struct TRtt {
	char* path_ocu_apa;
	char* path_ocu_doc;
	char* path_arb;
	char* path_lista;
	char* path_lista_baja;
	size_t orden_arb;
	size_t block_lista;
	TArbolBM* arb;
	TListaInvertida* lista;
};

typedef struct{
	long id_pa;
	unsigned int id_text;
} Ocurrencia;

int sort_cmp_ocu_doc(void* reg1, size_t size1, void* reg2, size_t size2){
	Ocurrencia *ent1 = reg1, *ent2 = reg2;
	if(ent1->id_pa > ent2->id_pa)
		return -1;
	else if(ent1->id_pa < ent2->id_pa)
		return 1;
	else{
		if(ent1->id_text > ent2->id_text)
			return -1;
		else if(ent1->id_text < ent2->id_text)
			return 1;
	}

	return 0;
}

/** Devuelve palabra o null
 */
static char* _getPalabra(char* texto, size_t *offset);

static int _getId(TArchivoReg* arch, char* p, long* id);

TRtt* Rtt_crear(char* path_ocu_apa, char* path_ocu_doc, char* path_arb, char* path_lista, char* path_lista_baja, size_t orden_arb, size_t block_lista){
	TRtt* this;
	if(!path_ocu_apa || ! path_ocu_doc || ! path_arb || ! orden_arb)
		return NULL;

	this = (TRtt*) calloc(1, sizeof(TRtt));

	this->path_ocu_apa = strcpy(malloc(strlen(path_ocu_apa)+1), path_ocu_apa);
	this->path_ocu_doc = strcpy(malloc(strlen(path_ocu_doc)+1), path_ocu_doc);
	this->path_arb = strcpy(malloc(strlen(path_arb)+1), path_arb);
	this->path_lista = strcpy(malloc(strlen(path_lista)+1), path_lista);
	this->path_lista_baja = strcpy(malloc(strlen(path_lista_baja)+1), path_lista_baja);
	this->orden_arb = orden_arb;
	this->block_lista = block_lista;
	this->arb = Arbol_crear(this->path_arb, this->orden_arb);
	this->lista = ListaInvertida_crear(this->path_lista, this->path_lista_baja, this->block_lista);

	return this;
}

int Rtt_agregar_texto(TRtt* this, unsigned int id_text, char* texto){
	size_t offset = 0;
	if(!this || ! texto)
		return 1;

	TArchivoReg* arch = ArchivoReg_crear(this->path_ocu_apa);
	ArchivoReg_seek_end(arch);
	TArchivo* arch_doc = Archivo_crear(this->path_ocu_doc, 512);
	Archivo_bloque_seek(arch_doc, 0, SEEK_END);

	char* palabra;
	while( (palabra = _getPalabra(texto, &offset)) ){
		long id;
		//TODO: ver si se ignora palabra;
		if(_getId(arch, palabra, &id)){ // Hay que agregar
			ArchivoReg_escribir(arch, (uint8_t*) palabra, strlen(palabra)+1);
			long ref_lista = ListaInvertida_new(this->lista);
			Arbol_insertar(this->arb, id, ref_lista);
		}
		Ocurrencia ocur;
		ocur.id_pa = id;
		ocur.id_text = id_text;

		Archivo_agregar_buf(arch_doc, (uint8_t*) &ocur, sizeof(Ocurrencia));
		free(palabra);
	}

	ArchivoReg_destruir(arch);
	Archivo_flush(arch_doc);
	Archivo_destruir(arch_doc);

	ListaInvertida_escribir(this->lista);
	return 0;
}

int Rtt_generar_indice(TRtt* this){
	if(!this)
		return 1;

	char * sorted_file = (char*) malloc(strlen(this->path_ocu_doc)+6);
	sprintf(sorted_file, "%s.sort", this->path_ocu_doc);
	sort_externo(512, this->path_ocu_doc, sorted_file, &sort_cmp_ocu_doc);

	TArchivo* arch_doc = Archivo_crear(this->path_ocu_doc, 512);
	Ocurrencia* occ;
	size_t size;
	Archivo_bloque_leer(arch_doc);
	long lista_ref;
	while((occ = (Ocurrencia*) Archivo_get_buf(arch_doc, &size))){
		Arbol_get(this->arb, occ->id_pa, &lista_ref);
		ListaInvertida_set(this->lista, lista_ref);
		ListaInvertida_agregar(this->lista, (uint8_t*) &(occ->id_text), sizeof(long));
		ListaInvertida_escribir(this->lista);
		free(occ);
	}

	free(sorted_file);
	remove(sorted_file);
	remove(this->path_ocu_doc);
	Archivo_destruir(arch_doc);
	ListaInvertida_escribir(this->lista);

	return 0;
}

long* Rtt_buscar(TRtt* this, char* t, size_t* len){
	long* ret=NULL;
	if(!this)
		return NULL;

	TArchivoReg* arch = ArchivoReg_crear(this->path_ocu_apa);
	long id;
	_getId(arch, t, &id);
	long ref_lista;
	if(Arbol_get(this->arb, id, &ref_lista) == 0){
		ListaInvertida_set(this->lista, ref_lista);
		//TODO: reemplazar por lista
		long aux[256];
		int i=0;
		size_t aux_s;
		uint8_t* aux_b;
		while( (aux_b = ListaInvertida_get(this->lista, &aux_s))){
			aux[i++] = * ((long*) aux_b);
			free(aux_b);
		}
		ret = (long*) malloc(sizeof(long) * i);
		*len = i;

		int k;
		for(k=0; k < i; k++)
			ret[k] = aux[k];
	}

	ArchivoReg_destruir(arch);
	return ret;
}

int Rtt_destruir(TRtt* this){
	if(!this)
		return 1;

	free(this->path_ocu_apa);
	free(this->path_ocu_doc);
	free(this->path_arb);
	free(this->path_lista);
	free(this->path_lista_baja);

	Arbol_destruir(this->arb);

	ListaInvertida_destruir(this->lista);
	free(this);
	return 0;
}

int Rtt_remover_termino(TRtt* this, char* t){
	if(!this)
		return 1;

	long id = 0;
	TArchivoReg* arch = ArchivoReg_crear(this->path_ocu_apa);
	int ret = _getId(arch, t, &id);
	ArchivoReg_destruir(arch);
	if(ret) // No existe
		return 1;

	Arbol_remover(this->arb, id);
	//TODO: borrarlo de la lista

	return 0;
}

static char* _getPalabra(char* texto, size_t *offset){
	//TODO: solo se contempla division por espacion, nada de \n \t o esas cosas
	size_t count, i;
	char* ret;
	char * txt_o = texto;
	texto += *offset;

	if(*texto == 0)
		return NULL;

	for(count = 0; texto[count] != 0 && texto[count] != ' ';count++){
	};

	if(count == 0){
		(*offset)++;
		return _getPalabra(txt_o, offset);
	}

	ret = (char*) malloc(count);
	ret[count] = 0;

	for(i=0; i < count; i++)
		ret[i] = tolower(texto[i]);

	*offset += count;

	return ret;
}

static int _getId(TArchivoReg* arch, char* p, long* id){
	uint8_t* buf;
	size_t size;
	*id = 1;
	ArchivoReg_seek_start(arch);
	while( (buf = ArchivoReg_leer(arch, &size)) ){
		if(strcmp((char*) buf, p) == 0){
			free(buf);
			return 0;
		}
		(*id)++;
		free(buf);
	}
	ArchivoReg_seek_end(arch);
	return 1;
}
