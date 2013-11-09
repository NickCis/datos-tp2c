#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "arbolbmas.h"
#include "archivo_bloque.h"
#include "serializador.h"

struct TArbolBM {
	TArchivo* arch;
	char *path;
	long root_bloque;
	long alloc_bloque;
	long corriente_bloque;
	short corriente_id;
	short corriente_ptr;
	size_t block_size;
	size_t order;
};

typedef struct {
	short hoja;
	short cant;
	long *ids; // [Orden -1]
	long *ptrs; // [Orden]
} TNodo;

static TNodo* _nodo_malloc(TArbolBM * this){
	TNodo* nodo = (TNodo*) malloc(sizeof(TNodo));
	nodo->ids = (long*) malloc(sizeof(long) * (this->order-1));
	nodo->ptrs = (long*) malloc(sizeof(long) * this->order);
	return nodo;
}

static void _nodo_free(TNodo* nodo){
	free(nodo->ids);
	free(nodo->ptrs);
	free(nodo);
}

static void _leerNodo(TArbolBM * this, size_t bloque, TNodo* nodo) {
	// TODO: checkear errores
	size_t size_read = 0;
	Archivo_bloque_seek(this->arch, bloque, SEEK_SET);
	Archivo_bloque_leer(this->arch);
	short *aux = (short*) Archivo_get_bloque_buf(this->arch, &size_read);
	nodo->hoja = aux[0];
	nodo->cant = aux[1];
	uint8_t* aux_ptr = (uint8_t*) &(aux[2]);
	memcpy(nodo->ids, aux_ptr, sizeof(long) * (this->order-1));
	memcpy(nodo->ptrs, aux_ptr+sizeof(long) * (this->order-1), sizeof(long) * this->order);
	free(aux);
}

static void _escribirNodo(TArbolBM * this, size_t bloque, TNodo* nodo) {
	// TODO: checkear errores
	size_t size = 0;
	SerializadorData data = {0};

	Archivo_bloque_seek(this->arch, bloque, SEEK_SET);
	Archivo_bloque_new(this->arch);

	data.size = sizeof(short);
	data.buf = (uint8_t*) &(nodo->hoja);
	uint8_t* buf =  Serializador_pack(NULL, SER_FIX_BUF, &data, &size);

	data.buf = (uint8_t*) &(nodo->cant);
	buf =  Serializador_pack(buf, SER_FIX_BUF, &data, &size);

	data.size = sizeof(long) * (this->order -1 );
	data.buf = (uint8_t*) nodo->ids;
	buf =  Serializador_pack(buf, SER_FIX_BUF, &data, &size);

	data.size += sizeof(long);
	data.buf = (uint8_t*) nodo->ptrs;
	buf =  Serializador_pack(buf, SER_FIX_BUF, &data, &size);

	if(ArchivoFijo_agregar_buf(this->arch, buf)){
		printf("error agregandoooo\n");
	}
	Archivo_flush(this->arch);

	free(buf);
}

static void _leerCabecera(TArbolBM *this) {
	// TODO: checkear errores
	size_t size;

	Archivo_bloque_seek(this->arch, 0, SEEK_SET);
	Archivo_bloque_leer(this->arch);

	long *aux = (long*) Archivo_get_bloque_buf(this->arch, &size);
	this->root_bloque = aux[0];

	this->alloc_bloque = aux[1];
	free(aux);
}

static void _escribirCabecera(TArbolBM *this) {
	// TODO: checkear errores
	long data[2* this->order];
	Archivo_bloque_seek(this->arch, 0, SEEK_SET);
	Archivo_bloque_new(this->arch);
	data[0] = this->root_bloque;
	data[1] = this->alloc_bloque;
	if(ArchivoFijo_agregar_buf(this->arch, (uint8_t*) data)){
		printf("error agregandoooo head\n");
	}
	Archivo_flush(this->arch);
}

TArbolBM* Arbol_crear(char *path, size_t order){
	int j;
	TArbolBM * this = (TArbolBM*) calloc(1, sizeof(TArbolBM));

	this->path = strcpy(malloc(strlen(path)+1), path);
	this->corriente_bloque = -1;
	this->order = order;
	// 4 * orden para punteros + 4 * (ordern -1) para llaves + (2) es hoja + (2) cuenta de llaves + 4 prefijo longitud
	this->block_size = 2 * order * sizeof(long) +  sizeof(size_t);

	this->arch = ArchivoFijo_crear(path, this->block_size, this->block_size - sizeof(size_t));

	if(Archivo_cant_bloque(this->arch)){
		_leerCabecera(this);
	} else{
		TNodo* nodo = _nodo_malloc(this);
		this->root_bloque = 1;
		this->alloc_bloque = 2;
		_escribirCabecera(this);
		nodo->hoja = 1;
		nodo->cant = 0;
		for(j = 0; j < order-1; j++)
		{ 
			nodo->ptrs[j] = -1; 
			nodo->ids[j] = 0; 
		}  
		nodo->ptrs[order - 1] = -1;
		_escribirNodo(this, 1, nodo);
		_nodo_free(nodo);

	}
	return this;
}

void Arbol_destruir(TArbolBM* this){
	// TODO: cerrar arch
	_escribirCabecera(this);
	Archivo_destruir(this->arch);
	free(this->path);
	free(this);
}

/** Devuelve indice de id (en array).
 */
static int buscarId(TNodo *nodo, long id) {
	int k;
	for(k = 0; k < nodo->cant; k++)
		if(nodo->ids[k] > id)
			break;

	return k;
}

static void buscarEnNodo(TArbolBM* this, TNodo *nodo, long *id, long *ptr) {
	*ptr = -1;
	if(this->corriente_bloque >= 0) {
		long sig_ptr = 0;
		*id = nodo->ids[this->corriente_id-1];
		*ptr = nodo->ptrs[this->corriente_id];

		if( sig_ptr < 0) {
			this->corriente_ptr = 0;
			this->corriente_id++;
			if(this->corriente_id > nodo->cant) {
				this->corriente_bloque = nodo->ptrs[0];
				this->corriente_id = 1;
			}
		}
	}
}

long Arbol_get(TArbolBM* this, long id) {
	TNodo *nodo = _nodo_malloc(this);
	long ptr = 0;
	this->corriente_bloque = this->root_bloque; // Seteo bloque correinte como raiz
	for(;;) {
		_leerNodo(this, this->corriente_bloque, nodo);
		this->corriente_id = buscarId(nodo, id);

		if(nodo->hoja)
			break;

		this->corriente_bloque = nodo->ptrs[this->corriente_id];
	}
	this->corriente_ptr = 0;

	if(this->corriente_id == 0)
		this->corriente_bloque = -1;

	buscarEnNodo(this, nodo, &id, &ptr);

	_nodo_free(nodo);
	return ptr;
}

static int _insertarId(TArbolBM* this, TNodo *nodo, int k_id, long *id, long *ptr) {
	long ids[this->order], ptrs[this->order+1];
	int count, count1, count2, k;
	count = nodo->cant + 1;
	count1 = count < this->order ? count : this->order/2;
	count2 = count - count1;
	for(k = this->order/2; k < k_id; k++) {
		ids[k] = nodo->ids[k];
		ptrs[k+1] = nodo->ptrs[k+1];
	}
	ids[k_id] = *id;
	ptrs[k_id+1] = *ptr;
	for(k = k_id; k < nodo->cant; k++) {
		ids[k+1] = nodo->ids[k];
		ptrs[k+2] = nodo->ptrs[k+1];
	}
	for(k = k_id; k < count1; k++) {
		nodo->ids[k] = ids[k];
		nodo->ptrs[k+1] = ptrs[k+1];
	}
	nodo->cant = count1;
	if(count2) {
		int s, d;
		TNodo* nnodo = _nodo_malloc(this);
		nnodo->hoja = nodo->hoja;
		count2 -= !nodo->hoja;
		for(s = this->order/2 + !nodo->hoja, d = 0; d < count2; s++, d++) {
			nnodo->ids[d] = ids[s];
			nnodo->ptrs[d] = ptrs[s];
		}
		nnodo->ptrs[d] = ptrs[s];
		nnodo->cant = count2;
		*id = ids[this->order/2];
		*ptr = this->alloc_bloque++;
		if(nodo->hoja) {  /* insert in sequential linked list */
			nnodo->ptrs[0] = nodo->ptrs[0];
			nodo->ptrs[0] = *ptr;
		}
		_escribirNodo(this, *ptr, nnodo);
		_escribirCabecera(this);
		_nodo_free(nnodo);
	}
	return count2;
}

/**
 *
 * @param this[in]: instancia de arbol
 * @param block[in]: numero de bloque
 * @param *id
 */
static int _recInsertar(TArbolBM* this, long block, long *id, long *ptr, int *error) {
	TNodo *nodo = _nodo_malloc(this);
	int k_id, dividir = 0;
	int misma_id;

	_leerNodo(this, block, nodo);
	k_id = buscarId(nodo, *id);
	misma_id = k_id && nodo->ids[k_id-1] == *id;

	if(!nodo->hoja)
		dividir = _recInsertar(this, nodo->ptrs[k_id], id, ptr, error);

	if(dividir || (nodo->hoja && !misma_id)) {
		dividir = _insertarId(this, nodo, k_id, id, ptr);
		_escribirNodo(this, block, nodo);
	} else if(nodo->hoja) {
		*error = -1;
	}

	_nodo_free(nodo);
	return dividir;
}

int Arbol_insertar(TArbolBM* this, long id, long ptr){
	int dividir;
	int error = 0;

	dividir = _recInsertar(this, this->root_bloque, &id, &ptr, &error);

	if(dividir) {
		TNodo* nodo = _nodo_malloc(this);
		nodo->hoja = 0;
		nodo->cant = 1;
		nodo->ids[0] = id;
		nodo->ptrs[1] = ptr;
		nodo->ptrs[0] = this->root_bloque;
		this->root_bloque = this->alloc_bloque++;
		_escribirNodo(this, this->root_bloque, nodo);
		_escribirCabecera(this);
		_nodo_free(nodo);
	}
	this->corriente_bloque = -1;
	return error;
}

/** Remover elemento.
 * 1) Arranco desde raiz, y busco hoja L donde pertence
 * 2) Remuevo elemento
 * 3) 
 */
int Arbol_remover(TArbolBM* this, long id){
	TNodo *nodo = _nodo_malloc(this);

	this->corriente_bloque = this->root_bloque; // Seteo bloque correinte como raiz
	for(;;) {
		_leerNodo(this, this->corriente_bloque, nodo);
		this->corriente_id = buscarId(nodo, id);

		if(nodo->hoja)
			break;

		this->corriente_bloque = nodo->ptrs[this->corriente_id];
	}
	this->corriente_ptr = 0;
	// this->corriente_bloque es la hoja donde esta
	int i;
	for(i=0; i < this->order - 1; i++){
		printf("leaf %d nodo->ids[%d] %ld nodo->ptrs[%d] %ld\n", nodo->hoja, i,  nodo->ids[i], i, nodo->ptrs[i]);
	}
	printf("nodo->ptrs[63] %ld\n", nodo->ptrs[63]);
	_nodo_free(nodo);
	return 0;
}
