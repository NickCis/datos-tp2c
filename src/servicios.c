#include "servicios.h"
#include <string.h>
#include "hash_extensible.h"
#include "config.h"
#include "serializador.h"
#include "autoincrement.h"
#include "arbolbmas.h"
#include "lista_invertida.h"
#include "rtt.h"

static THashExtensible* hash_servicios = NULL;
static unsigned int servicios_last_id = 0;
static TArbolBM* indice_cat = NULL;

struct TServicio {
	unsigned int id;
	unsigned int dni_p;
	char *nombre;
	char *descripcion;
	char tipo;
};

unsigned int Servicios_get_id(uint8_t* ele, size_t size){
	unsigned int id = * ( (unsigned int*) ele);
	return id;
}

static TRtt* _rtt(){
	return Rtt_crear(
		SERVICIOS_RTT_OCU_APA,
		SERVICIOS_RTT_OCU_DOC,
		SERVICIOS_RTT_ARB,
		SERVICIOS_RTT_LISTA,
		SERVICIOS_RTT_LISTA_BAJA,
		SERVICIOS_RTT_ARB_ORDEN,
		SERVICIOS_RTT_BLOCK
	);
}


/** Agrega un elemento al indice.
 */
static void _insertarIdIndiceCant(unsigned int id, unsigned id_cat){
	long lista_ref;
	TListaInvertida* lista = ListaInvertida_crear(SERVICIOS_LISTA_CAT, SERVICIOS_LISTA_CAT_BAJA, SERVICIOS_LISTA_BLOCK);

	if(Arbol_get(indice_cat, id_cat, &lista_ref)){
		lista_ref = ListaInvertida_new(lista);
		Arbol_insertar(indice_cat, id_cat, lista_ref);
	}

	ListaInvertida_set(lista, lista_ref);
	ListaInvertida_agregar(lista, (uint8_t*) &(id), sizeof(unsigned int));
	ListaInvertida_escribir(lista);
	ListaInvertida_destruir(lista);
}

/** Devuelve un TServicio sacando la info desde buf
 */
static TServicio* _servicioDesdeBuf(uint8_t* buf, size_t size);

/** Serializa la data, y devuelve un buffer, size es un parametro de salida, tama~no del buffer.
 * Hay que liberar la salida
 */
static uint8_t* _servBufDesdeData(
	unsigned int id,
	unsigned int dni_p,
	char *nombre,
	char *descripcion,
	char tipo,
	size_t *size_out
);

int Servicios_init(){
	hash_servicios = HashExtensible_crear(
		SERVICIOS_HASH_BLOQUE_PATH,
		SERVICIOS_HASH_TABLA_PATH,
		SERVICIOS_HASH_BAJA_PATH,
		SERVICIOS_HASH_BLOQUE_SIZE,
		&HashDispersionModulo,
		&Servicios_get_id
	);

	if(!hash_servicios)
		return 1;

	servicios_last_id = getLastId(SERVICIOS_LAST_ID_FILE);

	indice_cat = Arbol_crear(SERVICIOS_INDICE_CAT_PATH, SERVICIOS_ARBM_CAT_ORDEN);

	return 0;
}

int Servicios_end(){
	HashExtensible_destruir(hash_servicios);
	writeLastId(SERVICIOS_LAST_ID_FILE, servicios_last_id);
	hash_servicios = NULL;
	servicios_last_id = 0;

	Arbol_destruir(indice_cat);
	indice_cat = NULL;
	return 0;
}

// TODO: falta lo de lista invertida para desc
TServicio* Servicio_new(
	unsigned int dni_prov,
	char* nombre,
	char* desc,
	char tipo
){
	unsigned int last_id = servicios_last_id;
	size_t size = 0;
	uint8_t* buf = _servBufDesdeData(
		last_id+1,
		dni_prov,
		nombre,
		desc,
		tipo,
		&size
	);

	if(HashExtensible_insertar(hash_servicios, buf, size)){
		free(buf);
		return NULL;
	}

	TServicio* serv = _servicioDesdeBuf(buf, size);
	free(buf);

	TRtt* rtt = _rtt();
	Rtt_agregar_texto(rtt, last_id+1, desc);
	Rtt_generar_indice(rtt);
	Rtt_destruir(rtt);

	servicios_last_id++;
	return serv;
}

static uint8_t* _servBufDesdeData(
	unsigned int id,
	unsigned int dni_p,
	char *nombre,
	char *descripcion,
	char tipo,
	size_t *size_out
){
	size_t size = 0;
	SerializadorData data = {0};

	// Serializo id
	uint8_t *buf = Serializador_pack(NULL, SER_INT, (SerializadorData*) id, &size);

	// Serializo dni_p
	buf = Serializador_pack(buf, SER_INT, (SerializadorData*) dni_p, &size);

	// Serializo nombre
	data.buf = (uint8_t*) nombre;
	data.size = strlen(nombre) + 1;
	buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);

	// Serializo descripcion
	data.buf = (uint8_t*) descripcion;
	data.size = strlen(descripcion) + 1;
	buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);

	// t_u
	int tipo_int = tipo;
	buf = Serializador_pack(buf, SER_CHAR, (SerializadorData*) tipo_int, &size);

	*size_out = size;
	return buf;
}

static TServicio* _servicioDesdeBuf(uint8_t* buf, size_t buf_size){
	TServicio* this = (TServicio*) calloc(1, sizeof(TServicio));
	SerializadorData data;
	size_t size = 0;
	size_t pos = 0;
	data.buf = buf;
	data.size = buf_size;

	//id
	this->id = * ( (unsigned int*) Serializador_unpack(&data, SER_INT, &size, &pos));
	//dni_p
	this->dni_p = * ( (unsigned int*) Serializador_unpack(&data, SER_INT, &size, &pos));

	//Nombre
	char * str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
	this->nombre = strcpy(malloc(size), str_aux);

	//descripcion
	str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
	this->descripcion = strcpy(malloc(size), str_aux);

	//tipo
	this->tipo = * ( (char*) Serializador_unpack(&data, SER_CHAR, &size, &pos));

	return this;
}

int Servicio_free(TServicio* this){
	if(!this)
		return 1;

	free(this->nombre);
	free(this->descripcion);
	free(this);
	return 0;
}

TServicio* Servicio_from_id(unsigned int id){
	uint8_t* buf;
	size_t size;

	if( ! (buf = HashExtensible_get(hash_servicios, id, &size))){
		return NULL;
	}

	TServicio* serv = _servicioDesdeBuf(buf, size);
	free(buf);

	return serv;
}

TServicio* Servicio_from_dni_prov(unsigned int dni_prov, unsigned int *id_p){
	unsigned int last_id = servicios_last_id;
	if(*id_p >= last_id)
		return NULL;

	TServicio* serv = NULL;
	do {
		free(serv);
		serv = Servicio_from_id(++(*id_p));
	}while(dni_prov != Servicio_get_dni_p(serv));

	return serv;
}

unsigned int Servicio_get_id(TServicio* this){
	if(!this)
		return 0;
	return this->id;
}
unsigned int Servicio_get_dni_p(TServicio* this){
	if(!this)
		return 0;
	return this->dni_p;
}
const char* Servicio_get_nombre(TServicio* this){
	if(!this)
		return NULL;
	return this->nombre;
}
const char* Servicio_get_descripcion(TServicio* this){
	if(!this)
		return NULL;
	return this->descripcion;
}
char Servicio_get_tipo(TServicio* this){
	if(!this)
		return 0;
	return this->tipo;
}

TServicio* Servicio_del(unsigned int id){
	size_t size;
	uint8_t* buf = HashExtensible_del(hash_servicios, id, &size);
	TServicio* serv = _servicioDesdeBuf(buf, size);
	free(buf);

	return serv;
}

int Servicio_agregar_categoria(unsigned int id, unsigned int id_cat){
	_insertarIdIndiceCant(id, id_cat);
	return 0;
}

unsigned int* Servicio_from_categoria(unsigned int id_cat, size_t *len){
	long lista_ref;
	if(Arbol_get(indice_cat, id_cat, &lista_ref)){
		return NULL;
	}

	TListaInvertida* lista = ListaInvertida_crear(SERVICIOS_LISTA_CAT, SERVICIOS_LISTA_CAT_BAJA, SERVICIOS_LISTA_BLOCK);

	ListaInvertida_set(lista, lista_ref);

	unsigned int aux[255];
	uint8_t* aux_b;
	size_t aux_s;
	int i = 0;

	while( (aux_b = ListaInvertida_get(lista, &aux_s))){
		aux[i++] = * ((unsigned int*) aux_b);
		free(aux_b);
	}

	unsigned int *ret = (unsigned int*) malloc(sizeof(unsigned int) * i);
	*len = i;
	int k;
	for(k=0 ; k < i ; k++)
		ret[k] = aux[k];

	return ret;
}

unsigned int* Servicio_buscar(char* t, size_t* len){
	unsigned int* ret;
	TRtt* rtt = _rtt();
	ret = (unsigned int*) Rtt_buscar(rtt, t, len);
	Rtt_destruir(rtt);
	return ret;
}
