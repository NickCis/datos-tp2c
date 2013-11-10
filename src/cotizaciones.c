#include <string.h>
#include "cotizaciones.h"
#include "hash_extensible.h"
#include "config.h"
#include "serializador.h"
#include "autoincrement.h"

// Hash usado para guardar en disco a las cotizaciones
static THashExtensible* hash_cotizaciones = NULL;
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

	return 0;
}

int Cotizaciones_end(){
	HashExtensible_destruir(hash_cotizaciones);
	writeLastId(COTIZACIONES_LAST_ID_FILE, cotizaciones_last_id);

	hash_cotizaciones = NULL;
	cotizaciones_last_id = 0;
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
	size_t size = 0;
	SerializadorData data = {0};

	// Serializo id
	uint8_t *buf = Serializador_pack(NULL, SER_INT, (SerializadorData*) id, &size);

	// Serializo id_serv
	buf = Serializador_pack(buf, SER_INT, (SerializadorData*) id_serv, &size);

	// Serializo dni
	buf = Serializador_pack(buf, SER_INT, (SerializadorData*) dni, &size);

	// Serializo pedido
	data.buf = (uint8_t*) pedido;
	data.size = strlen(pedido) + 1;
	buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);

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

	//Pedido
	char * str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
	this->pedido = strcpy(malloc(size), str_aux);

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
