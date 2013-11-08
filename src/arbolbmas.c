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
	short corriente_llave;
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

	//if(Archivo_bloque_agregar_buf(this->arch, buf, size)){
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
	//free(aux);

	//aux = (long*) Archivo_get_bloque_buf(this->arch, &size);
	this->alloc_bloque = aux[1];
	free(aux);
}

static void _escribirCabecera(TArbolBM *this) {
	// TODO: checkear errores
	long data[2* this->order];
	Archivo_bloque_seek(this->arch, 0, SEEK_SET);
	Archivo_bloque_new(this->arch);
	//Archivo_bloque_agregar_buf(this->arch, (uint8_t*) &(this->root_bloque), sizeof(long));
	//Archivo_bloque_agregar_buf(this->arch, (uint8_t*) &(this->alloc_bloque), sizeof(long));
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
static int FindKey(TNodo *nodo, long id) {
	int k;
	for(k = 0; k < nodo->cant; k++)
		if(nodo->ids[k] > id)
			break;

	return k;
}

static void CheckBucket(TArbolBM* this, TNodo *Node, long *Key, long *Ptr) {
	*Ptr = -1;
	if(this->corriente_bloque >= 0) {
		long NextPtr = 0;
		*Key = Node->ids[this->corriente_llave-1];
		*Ptr = Node->ptrs[this->corriente_llave];

		if( NextPtr < 0) {
			this->corriente_ptr = 0;
			this->corriente_llave++;
			if(this->corriente_llave > Node->cant) {
				this->corriente_bloque = Node->ptrs[0];
				this->corriente_llave = 1;
			}
		}
	}
}

long Arbol_get(TArbolBM* this, long id) {
	TNodo *nodo = _nodo_malloc(this);
	long Ptr = 0;
	this->corriente_bloque = this->root_bloque; // Seteo bloque correinte como raiz
	for(;;) {
		_leerNodo(this, this->corriente_bloque, nodo);
		this->corriente_llave = FindKey(nodo, id);

		if(nodo->hoja)
			break;

		this->corriente_bloque = nodo->ptrs[this->corriente_llave];
	}
	this->corriente_ptr = 0;

	if(this->corriente_llave == 0)
		this->corriente_bloque = -1;

	CheckBucket(this, nodo, &id, &Ptr);

	_nodo_free(nodo);
	return Ptr;
}

static int InsertKey(TArbolBM* this, TNodo *nodo, int KIdx, long *Key, long *Ptr) {
	long ids[this->order], ptrs[this->order+1];
	int Count, Count1, Count2, k;
	Count = nodo->cant + 1;
	Count1 = Count < this->order ? Count : this->order/2;
	Count2 = Count - Count1;
	for(k = this->order/2; k < KIdx; k++) {
		ids[k] = nodo->ids[k];
		ptrs[k+1] = nodo->ptrs[k+1];
	}
	ids[KIdx] = *Key;
	ptrs[KIdx+1] = *Ptr;
	for(k = KIdx; k < nodo->cant; k++) {
		ids[k+1] = nodo->ids[k];
		ptrs[k+2] = nodo->ptrs[k+1];
	}
	for(k = KIdx; k < Count1; k++) {
		nodo->ids[k] = ids[k];
		nodo->ptrs[k+1] = ptrs[k+1];
	}
	nodo->cant = Count1;
	if(Count2) {
		int s, d;
		TNodo* nnodo = _nodo_malloc(this);
		nnodo->hoja = nodo->hoja;
		Count2 -= !nodo->hoja;
		for(s = this->order/2 + !nodo->hoja, d = 0; d < Count2; s++, d++) {
			nnodo->ids[d] = ids[s];
			nnodo->ptrs[d] = ptrs[s];
		}
		nnodo->ptrs[d] = ptrs[s];
		nnodo->cant = Count2;
		*Key = ids[this->order/2];
		*Ptr = this->alloc_bloque++;
		if(nodo->hoja) {  /* insert in sequential linked list */
			nnodo->ptrs[0] = nodo->ptrs[0];
			nodo->ptrs[0] = *Ptr;
		}
		_escribirNodo(this, *Ptr, nnodo);
		_escribirCabecera(this);
		_nodo_free(nnodo);
	}
	return Count2;
}

/** 
 *
 * @param this[in]: instancia de arbol
 * @param Block[in]: numero de bloque
 * @param *Key
 */
static int RecInsert(TArbolBM* this, long Block, long *Key, long *Ptr, int *error) {
	TNodo *nodo = _nodo_malloc(this);
	int KIdx, Split = 0;
	int EqualKey;

	_leerNodo(this, Block, nodo);
	KIdx = FindKey(nodo, *Key);
	EqualKey = KIdx && nodo->ids[KIdx-1] == *Key;

	if(!nodo->hoja)
		Split = RecInsert(this, nodo->ptrs[KIdx], Key, Ptr, error);

	if(Split || (nodo->hoja && !EqualKey)) {
		Split = InsertKey(this, nodo, KIdx, Key, Ptr);
		_escribirNodo(this, Block, nodo);
	} else if(nodo->hoja) {
		*error = -1;
	}

	_nodo_free(nodo);
	return Split;
}

int Arbol_insertar(TArbolBM* this, long Key, long Ptr){
	int Split;
	int error = 0;

	Split = RecInsert(this, this->root_bloque, &Key, &Ptr, &error);

	if(Split) {
		TNodo* nodo = _nodo_malloc(this);
		nodo->hoja = 0;
		nodo->cant = 1;
		nodo->ids[0] = Key;
		nodo->ptrs[1] = Ptr;
		nodo->ptrs[0] = this->root_bloque;
		this->root_bloque = this->alloc_bloque++;
		_escribirNodo(this, this->root_bloque, nodo);
		_escribirCabecera(this);
		_nodo_free(nodo);
	}
	this->corriente_bloque = -1;
	return error;
}
