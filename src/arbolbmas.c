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
	size_t orden;
};

/** Si es hoja, primer elemnto de ptr es el numero del bloque del siguiente bloque, segun orden de id, o -1 si es el ultimo bloque.
 * Si no es hoja, la estructura es:
 *        ids[0]    ids[1]
 *  ptrs[0] | ptrs[1] | ptrs[2]
 * con ptrs[n] numero de bloque.
 * cant tiene el largo del array ids (siempre ptrs va a tener de largo cant +1)
 */
typedef struct {
	short hoja;
	short cant;
	long *ids; // [Orden -1]
	long *ptrs; // [Orden]
} TNodo;

static TNodo* _nodo_malloc(TArbolBM * this){
	TNodo* nodo = (TNodo*) malloc(sizeof(TNodo));
	nodo->ids = (long*) malloc(sizeof(long) * (this->orden-1));
	nodo->ptrs = (long*) malloc(sizeof(long) * this->orden);
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
	memcpy(nodo->ids, aux_ptr, sizeof(long) * (this->orden-1));
	memcpy(nodo->ptrs, aux_ptr+sizeof(long) * (this->orden-1), sizeof(long) * this->orden);
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

	data.size = sizeof(long) * (this->orden -1 );
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
	long data[2* this->orden];
	Archivo_bloque_seek(this->arch, 0, SEEK_SET);
	Archivo_bloque_new(this->arch);
	data[0] = this->root_bloque;
	data[1] = this->alloc_bloque;
	if(ArchivoFijo_agregar_buf(this->arch, (uint8_t*) data)){
		printf("error agregandoooo head\n");
	}
	Archivo_flush(this->arch);
}

TArbolBM* Arbol_crear(char *path, size_t orden){
	int j;
	TArbolBM * this = (TArbolBM*) calloc(1, sizeof(TArbolBM));

	this->path = strcpy(malloc(strlen(path)+1), path);
	this->corriente_bloque = -1;
	this->orden = orden;
	// 4 * orden para punteros + 4 * (orden -1) para llaves + (2) es hoja + (2) cuenta de llaves + 4 prefijo longitud
	this->block_size = 2 * orden * sizeof(long) +  sizeof(size_t);

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
		for(j = 0; j < orden-1; j++)
		{ 
			nodo->ptrs[j] = -1; 
			nodo->ids[j] = 0; 
		}  
		nodo->ptrs[orden - 1] = -1;
		_escribirNodo(this, 1, nodo);
		_nodo_free(nodo);

	}
	return this;
}

void Arbol_destruir(TArbolBM* this){
	_escribirCabecera(this);
	Archivo_destruir(this->arch);
	free(this->path);
	free(this);
}

/** Devuelve indice de id (en array).
 */
static int _buscarIdAprox(TNodo *nodo, long id) {
	int k;
	for(k = 0; k < nodo->cant; k++)
		if(nodo->ids[k] > id)
			break;

	return k;
}

static int _buscarIdExact(TNodo *nodo, long id) {
	int k;
	for(k = 0; k < nodo->cant; k++)
		if(nodo->ids[k] == id)
			return k;

	return -1;
}

static void _buscarEnNodo(TArbolBM* this, TNodo *nodo, long *id, long *ptr) {
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

int Arbol_get(TArbolBM* this, long id, long* ptr) {
	TNodo *nodo = _nodo_malloc(this);
	*ptr = 0;
	this->corriente_bloque = this->root_bloque; // Seteo bloque correinte como raiz
	for(;;) {
		_leerNodo(this, this->corriente_bloque, nodo);
		this->corriente_id = _buscarIdAprox(nodo, id);

		if(nodo->hoja)
			break;

		this->corriente_bloque = nodo->ptrs[this->corriente_id];
	}
	this->corriente_ptr = 0;

	if(this->corriente_id == 0 || _buscarIdExact(nodo, id) == -1){
		_nodo_free(nodo);
		return -1;
	}

	_buscarEnNodo(this, nodo, &id, ptr);
	_nodo_free(nodo);
	return 0;
}

static int _insertarId(TArbolBM* this, TNodo *nodo, int k_id, long *id, long *ptr) {
	long ids[this->orden], ptrs[this->orden+1];
	int count, count1, count2, k;
	count = nodo->cant + 1;
	// tama~no que va a tener finalmente el nodo
	count1 = count < this->orden ? count : this->orden/2;
	count2 = count - count1;

	// Creo vector ordenado con todos los ids y ptrs
	for(k = this->orden/2; k < k_id; k++) {
		ids[k] = nodo->ids[k];
		ptrs[k+1] = nodo->ptrs[k+1];
	}

	ids[k_id] = *id;
	ptrs[k_id+1] = *ptr;

	for(k = k_id; k < nodo->cant; k++) {
		ids[k+1] = nodo->ids[k];
		ptrs[k+2] = nodo->ptrs[k+1];
	}
	// --

	for(k = k_id; k < count1; k++) {
		nodo->ids[k] = ids[k];
		nodo->ptrs[k+1] = ptrs[k+1];
	}
	nodo->cant = count1;

	if(count2) { // Hay overflow!
		int s, d;
		TNodo* nnodo = _nodo_malloc(this);
		nnodo->hoja = nodo->hoja;
		count2 -= !nodo->hoja;
		for(s = this->orden/2 + !nodo->hoja, d = 0; d < count2; s++, d++) {
			nnodo->ids[d] = ids[s];
			nnodo->ptrs[d] = ptrs[s];
		}
		nnodo->ptrs[d] = ptrs[s];
		nnodo->cant = count2;
		*id = ids[this->orden/2];
		*ptr = this->alloc_bloque++;
		if(nodo->hoja) {  // inserto el nodo sigueinte para lista secuencial
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
	k_id = _buscarIdAprox(nodo, *id);
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

static int _removerId(TArbolBM* this, TNodo *nodo, int k_id){
	int i;
	int under = 0;
	if(nodo->cant == this->orden / 2)
		under = 1;

	//printf("Llamo a remover id k_id %d id: %ld\n", k_id, nodo->ids[k_id -1]);

	nodo->cant--;
	for(i=k_id-1; i < nodo->cant; i++){
		nodo->ids[i] = nodo->ids[i+1];
		nodo->ptrs[i+1] = nodo->ptrs[i+2];
	}

	return under;
}

static void _fusionarNodos(TNodo* nodo1, TNodo* nodo2){
	int i = 0;
	for(i=0; i < nodo2->cant; i++){
		nodo1->ids[nodo1->cant+i] = nodo2->ids[i];
		nodo1->ptrs[nodo1->cant+i+1] = nodo2->ptrs[i+1];
	}
	nodo1->ptrs[0] = nodo2->ptrs[0];
}

static int _recRemover(TArbolBM* this, long block, long *id, int *error){
	TNodo *nodo = _nodo_malloc(this);
	int k_id, under = 0;
	int misma_id;

	_leerNodo(this, block, nodo);
	k_id = _buscarIdAprox(nodo, *id);
	misma_id = k_id && nodo->ids[k_id-1] == *id;

	if(!nodo->hoja)
		under = _recRemover(this, nodo->ptrs[k_id], id, error);

	if(nodo->hoja) {
		if(misma_id){
			under = _removerId(this, nodo, k_id);
			_escribirNodo(this, block, nodo);
		}else{
			*error = -1;
		}
	}else if(under){
		int i;
		long mov_id;
		// Primero intento balancear
		// nodo->ptrs[k_id-/+1] es hermano (-)izq (+) der
		long hermano = k_id > 0 ? k_id -1 : k_id +1;

		TNodo *hernodo = _nodo_malloc(this);
		_leerNodo(this, nodo->ptrs[hermano], hernodo);

		TNodo *hijnodo = _nodo_malloc(this);
		_leerNodo(this, nodo->ptrs[k_id], hijnodo);

		if(hernodo->cant > this->orden /2){ // Puedo rebalancear
			if(hermano < k_id){ // izq
				for(i=hijnodo->cant; i > 0; i--){
					hijnodo->ids[i] = hijnodo->ids[i-1];
					hijnodo->ptrs[i+1] = hijnodo->ids[i];
				}
				hijnodo->cant++;
				mov_id = hernodo->ids[hernodo->cant-1];
				hijnodo->ids[0] = misma_id ? mov_id : nodo->ids[k_id];
				hijnodo->ptrs[1] = hernodo->ids[hernodo->cant];
				hernodo->cant--;
			}else{ // der
				mov_id = hernodo->ids[0];
				hijnodo->ids[hijnodo->cant] = misma_id ? mov_id : nodo->ids[k_id];;
				hijnodo->cant++;
				hijnodo->ptrs[hijnodo->cant] = hernodo->ids[1];

				for(i=0; i < hernodo->cant; i++){
					hernodo->ids[i] = hernodo->ids[i+1];
					hernodo->ptrs[i+1] = hernodo->ids[i+2];
				}
				hernodo->cant--;
			}
			nodo->ids[k_id] = mov_id;
		}else{ // Hay que fusionar
			// TODO: habria que guardar el nodo que se esta liberando, sino va a qedar como basura en el archivo
			if(hermano < k_id){
				_fusionarNodos(hernodo, hijnodo);
			}else{
				_fusionarNodos(hijnodo, hernodo);
			}

			under = _removerId(this, nodo, k_id);
		}

		_escribirNodo(this, block, nodo);
		_escribirNodo(this, nodo->ptrs[hermano], hernodo);
		_escribirNodo(this, nodo->ptrs[k_id], hijnodo);

		_nodo_free(hernodo);
		_nodo_free(hijnodo);

	}

	_nodo_free(nodo);
	return under;
}

/** Remover elemento.
 * 1) Arranco desde raiz, y busco hoja L donde pertence
 * 2) Remuevo elemento
 * 3) 
 *
 * @return Ok ->0, error (o no existe) =! 0
 */
int Arbol_remover(TArbolBM* this, long id){
	//int under;
	int error = 0;

	/*under =*/ _recRemover(this, this->root_bloque, &id, &error);

	return error;

	/*this->corriente_bloque = this->root_bloque; // Seteo bloque correinte como raiz
	for(;;) {
		_leerNodo(this, this->corriente_bloque, nodo);
		this->corriente_id = _buscarIdAprox(nodo, id);

		if(nodo->hoja)
			break;

		corriente_padre = this->corriente_bloque;
		this->corriente_bloque = nodo->ptrs[this->corriente_id];
	}
	this->corriente_ptr = 0;

	int k_id = _buscarIdExact(nodo, id);
	if(this->corriente_id == 0 || k_id == -1){ // Elemento no existe
		_nodo_free(nodo);
		return -1;
	}

	if(nodo->cant > this->orden / 2){ // Eliminacion facil
		int i;
		nodo->cant--;
		for(i=k_id; i < nodo->cant; i++){
			nodo->ids[k_id] = nodo->ids[k_id+1];
			nodo->ptrs[k_id+1] = nodo->ids[k_id+2];
		}

		_escribirNodo(this, this->corriente_bloque, nodo);
		_nodo_free(nodo);
		return 0;
	}

	printf("Eliminacion complicada, underflow \n");




	// this->corriente_bloque es la hoja donde esta
	printf("\nSoy bloque n: %ld cant:%d\n", this->corriente_bloque, nodo->cant);
	int i;
	for(i=0; i < nodo->cant - 1; i++){
		printf("leaf %d nodo->ids[%d] %ld nodo->ptrs[%d] %ld\n", nodo->hoja, i,  nodo->ids[i], i, nodo->ptrs[i]);
	}
	printf("nodo->ptrs[%d] %ld\n", i, nodo->ptrs[i]);

	if(corriente_padre != -1){
		_leerNodo(this, corriente_padre, nodo);

		printf("Corriente padre: %ld cant: %d hoja %d\n", corriente_padre, nodo->cant, nodo->hoja);
		for(i=0; i < nodo->cant; i++){
			printf("%ld | _%ld_ | ", nodo->ptrs[i], nodo->ids[i]);
		}
		printf("%ld\n", nodo->ptrs[i]);
	}
	_nodo_free(nodo);
	return 0;*/
}
