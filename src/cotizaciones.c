#include <string.h>
#include "cotizaciones.h"
#include "hash_extensible.h"
#include "config.h"
#include "serializador.h"
#include "autoincrement.h"
#include "arbolbmas.h"
#include "lista_invertida.h"
#include "cifrador_hill.h"
#include "servicios.h"
#include "usuarios.h"


// Hash usado para guardar en disco a las cotizaciones
static THashExtensible* hash_cotizaciones = NULL;
static TArbolBM* indice_id_serv = NULL;
static TArbolBM* indice_id_usu = NULL;
static unsigned int cotizaciones_last_id = 0;

struct TCotizacion {
	unsigned int id;
	unsigned int id_serv;
	unsigned int dni;
	char *pedido;
	char *fecha;
	char *hora;
};

unsigned int Cotizaciones_get_id(uint8_t* ele, size_t size){
	unsigned int id = * ( (unsigned int*) ele);
	return id;
}

/** Devuelve un TCotizacion sacando la info desde buf
 */
static TCotizacion* _cotizacionDesdeBuf(uint8_t* buf, size_t size);

/** Serializa la data, y devuelve un buffer, size es un parametro de salida, tama~no del buffer.
 * Hay que liberar la salida
 */
static uint8_t* _cotBufDesdeData(
	unsigned int id,
	unsigned int id_serv,
	unsigned int dni,
	char *pedido,
	char *fecha,
	char *hora,
	size_t *size_out
);

/** Agrega un elemento al indice.
 */
static void _insertarIdIndiceIdServ(unsigned int id, unsigned int id_serv){
	long lista_ref;
	TListaInvertida* lista = ListaInvertida_crear(COTIZACIONES_LISTA_SERV, COTIZACIONES_LISTA_SERV_BAJA, COTIZACIONES_LISTA_SERV_BLOCK);

	if(Arbol_get(indice_id_serv, id_serv, &lista_ref)){
		lista_ref = ListaInvertida_new(lista);
		Arbol_insertar(indice_id_serv, id_serv, lista_ref);
	}

	ListaInvertida_set(lista, lista_ref);
	ListaInvertida_agregar(lista, (uint8_t*) &(id), sizeof(unsigned int));
	ListaInvertida_escribir(lista);
	ListaInvertida_destruir(lista);
}

static void _insertarIdIndiceIdDni(unsigned int id, unsigned int dni){
	long lista_ref;
	TListaInvertida* lista = ListaInvertida_crear(COTIZACIONES_LISTA_DNI, COTIZACIONES_LISTA_DNI_BAJA, COTIZACIONES_LISTA_DNI_BLOCK);

	if(Arbol_get(indice_id_usu, dni, &lista_ref)){
		lista_ref = ListaInvertida_new(lista);
		Arbol_insertar(indice_id_usu, dni, lista_ref);
	}

	ListaInvertida_set(lista, lista_ref);
	ListaInvertida_agregar(lista, (uint8_t*) &(id), sizeof(unsigned int));
	ListaInvertida_escribir(lista);
	ListaInvertida_destruir(lista);
}

int Cotizaciones_init(){
	hash_cotizaciones = HashExtensible_crear(
		COTIZACIONES_HASH_BLOQUE_PATH,
		COTIZACIONES_HASH_TABLA_PATH,
		COTIZACIONES_HASH_BAJA_PATH,
		COTIZACIONES_HASH_BLOQUE_SIZE,
		&HashDispersionModulo,
		&Cotizaciones_get_id
	);

	if(!hash_cotizaciones)
		return 1;

	cotizaciones_last_id = getLastId(COTIZACIONES_LAST_ID_FILE);

	indice_id_serv = Arbol_crear(COTIZACIONES_INDICE_SERV_PATH, COTIZACIONES_ARBM_SERV_ORDEN);
	indice_id_usu = Arbol_crear(COTIZACIONES_INDICE_DNI_PATH, COTIZACIONES_ARBM_DNI_ORDEN);

	return 0;
}

int Cotizaciones_end(){
	HashExtensible_destruir(hash_cotizaciones);
	writeLastId(COTIZACIONES_LAST_ID_FILE, cotizaciones_last_id);

	hash_cotizaciones = NULL;
	cotizaciones_last_id = 0;

	Arbol_destruir(indice_id_serv);
	indice_id_serv = NULL;
	Arbol_destruir(indice_id_usu);
	indice_id_usu = NULL;
	return 0;
}

TCotizacion* Cotizacion_new(
	unsigned int id_serv,
	unsigned int dni,
	char* pedido,
	char* fecha,
	char* hora
){
	unsigned int last_id = cotizaciones_last_id;
	size_t size = 0;
	uint8_t* buf = _cotBufDesdeData(
		last_id+1,
		id_serv,
		dni,
		pedido,
		fecha,
		hora,
		&size
	);

	if(HashExtensible_insertar(hash_cotizaciones, buf, size)){
		free(buf);
		return NULL;
	}

	TCotizacion* cot = _cotizacionDesdeBuf(buf, size);
	free(buf);

	_insertarIdIndiceIdServ(last_id+1, id_serv);
	_insertarIdIndiceIdDni(last_id+1, dni);

	cotizaciones_last_id++;
	return cot;
}

static uint8_t* _cotBufDesdeData(
	unsigned int id,
	unsigned int id_serv,
	unsigned int dni,
	char *pedido,
	char *fecha,
	char *hora,
	size_t *size_out
){
	//TODO: cheqear errores
	size_t size = 0;
	SerializadorData data = {0};

	// Serializo id
	uint8_t *buf = Serializador_pack(NULL, SER_INT, (SerializadorData*) id, &size);

	// Serializo id_serv
	buf = Serializador_pack(buf, SER_INT, (SerializadorData*) id_serv, &size);

	// Serializo dni
	buf = Serializador_pack(buf, SER_INT, (SerializadorData*) dni, &size);


	// Obtengo servicio
	TServicio* serv =  Servicio_from_id(id_serv);
	TUsuario* prov = Usuario_from_dni(Servicio_get_dni_p(serv));
	Servicio_free(serv);

	char *pedido_n = (char*) calloc(1, strlen(pedido)+1);
	char *pedido_e = (char*) calloc(1, strlen(pedido)+1);
	normalizar(pedido, pedido_n);
	encrypt(Usuario_get_password(prov), pedido_n, pedido_e);
	// Serializo pedido
	data.buf = (uint8_t*) pedido_e;
	data.size = strlen(pedido_e) + 1;
	buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);
	Usuario_free(prov);
	free(pedido_e);
	free(pedido_n);

	// Serializo fecha
	data.buf = (uint8_t*) fecha;
	data.size = 8 + 1;
	buf = Serializador_pack(buf, SER_FIX_BUF, &data, &size);

	// Serializo hora
	data.buf = (uint8_t*) hora;
	data.size = 4 + 1;
	buf = Serializador_pack(buf, SER_FIX_BUF, &data, &size);

	*size_out = size;
	return buf;
}

static TCotizacion* _cotizacionDesdeBuf(uint8_t* buf, size_t buf_size){
	TCotizacion* this = (TCotizacion*) calloc(1, sizeof(TCotizacion));
	SerializadorData data;
	size_t size = 0;
	size_t pos = 0;
	data.buf = buf;
	data.size = buf_size;

	//id
	this->id = * ( (unsigned int*) Serializador_unpack(&data, SER_INT, &size, &pos));
	//id_serv
	this->id_serv = * ( (unsigned int*) Serializador_unpack(&data, SER_INT, &size, &pos));
	//dni
	this->dni = * ( (unsigned int*) Serializador_unpack(&data, SER_INT, &size, &pos));

	// Obtengo servicio
	TServicio* serv =  Servicio_from_id(this->id_serv);
	TUsuario* prov = Usuario_from_dni(Servicio_get_dni_p(serv));
	Servicio_free(serv);

	//Pedido
	char * str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
	this->pedido = strcpy(malloc(size), str_aux);
	decrypt(Usuario_get_password(prov), str_aux, this->pedido);
	Usuario_free(prov);

	//fecha
	size = 9;
	str_aux = (char*) Serializador_unpack(&data, SER_FIX_BUF, &size, &pos);
	this->fecha = strcpy(malloc(size), str_aux);

	//hora
	size = 5;
	str_aux = (char*) Serializador_unpack(&data, SER_FIX_BUF, &size, &pos);
	this->hora = strcpy(malloc(size), str_aux);

	return this;
}

int Cotizacion_free(TCotizacion* this){
	if(!this)
		return 1;

	free(this->pedido);
	free(this->fecha);
	free(this->hora);
	free(this);
	return 0;
}

TCotizacion* Cotizacion_from_id(unsigned int id){
	uint8_t* buf;
	size_t size;

	if( ! (buf = HashExtensible_get(hash_cotizaciones, id, &size))){
		return NULL;
	}

	TCotizacion* cot = _cotizacionDesdeBuf(buf, size);
	free(buf);

	return cot;
}

unsigned int Cotizacion_get_id(TCotizacion* this){
	if(!this)
		return 0;
	return this->id;
}
unsigned int Cotizacion_get_id_serv(TCotizacion* this){
	if(!this)
		return 0;
	return this->id_serv;
}
unsigned int Cotizacion_get_dni(TCotizacion* this){
	if(!this)
		return 0;
	return this->dni;
}
const char* Cotizacion_get_pedido(TCotizacion* this){
	if(!this)
		return NULL;
	return this->pedido;
}
const char* Cotizacion_get_fecha(TCotizacion* this){
	if(!this)
		return NULL;
	return this->fecha;
}
const char* Cotizacion_get_hora(TCotizacion* this){
	if(!this)
		return NULL;
	return this->hora;
}

TCotizacion* Cotizacion_del(unsigned int id){
	size_t size;
	uint8_t* buf = HashExtensible_del(hash_cotizaciones, id, &size);
	TCotizacion* cot = _cotizacionDesdeBuf(buf, size);
	free(buf);

	return cot;
}

unsigned int* Cotizacion_from_id_serv(unsigned int id_serv, size_t *len){
	long lista_ref;
	if(Arbol_get(indice_id_serv, id_serv, &lista_ref)){
		return NULL;
	}

	TListaInvertida* lista = ListaInvertida_crear(COTIZACIONES_LISTA_SERV, COTIZACIONES_LISTA_SERV_BAJA, COTIZACIONES_LISTA_SERV_BLOCK);

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

unsigned int* Cotizacion_from_id_usu(unsigned int id_usu, size_t *len){
	long lista_ref;
	if(Arbol_get(indice_id_usu, id_usu, &lista_ref)){
		return NULL;
	}

	TListaInvertida* lista = ListaInvertida_crear(COTIZACIONES_LISTA_DNI, COTIZACIONES_LISTA_DNI_BAJA, COTIZACIONES_LISTA_DNI_BLOCK);

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
