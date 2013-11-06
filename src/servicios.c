#include "servicios.h"
#include <string.h>
#include "hash_extensible.h"
#include "config.h"
#include "serializador.h"

THashExtensible* hash_servicios = NULL;

struct TServicio {
	unsigned int id;
	unsigned int dni_p;
	char *nombre;
	char *descripcion;
	char tipo;
};

unsigned int _getLastId(){
	unsigned int ret = 0;
	FILE* fd = fopen(SERVICIOS_LAST_ID_FILE, "rb");
	if(!fd)
		return 0;

	fread(&ret, sizeof(unsigned int), 1, fd);
	fclose(fd);
	return ret;
}

int _writeLastId(unsigned int id){
	FILE* fd = fopen(SERVICIOS_LAST_ID_FILE, "wb");
	if(!fd)
		return 1;

	fwrite(&id, sizeof(unsigned int), 1, fd);
	fclose(fd);
	return 0;
}

unsigned int Servicios_get_id(uint8_t* ele, size_t size){
	unsigned int id = * ( (unsigned int*) ele);
	return id;
}

/** Devuelve un TServicio sacando la info desde buf
 */
TServicio* _servicioDesdeBuf(uint8_t* buf, size_t size);

/** Serializa la data, y devuelve un buffer, size es un parametro de salida, tama~no del buffer.
 * Hay que liberar la salida
 */
uint8_t* _servBufDesdeData(
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

	return 0;
}

int Servicios_end(){
	HashExtensible_destruir(hash_servicios);
	hash_servicios = NULL;
	return 0;
}

// TODO: falta lo de lista invertida para desc
TServicio* Servicio_new(
	unsigned int dni_prov,
	char* nombre,
	char* desc,
	char tipo
){
	unsigned int last_id = _getLastId();
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

	_writeLastId(last_id+1);
	return serv;
}

uint8_t* _servBufDesdeData(
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

TServicio* _servicioDesdeBuf(uint8_t* buf, size_t buf_size){
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
	unsigned int last_id = _getLastId();
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

int Servicios_free(TServicio* this){
	if(!this)
		return 1;

	free(this->nombre);
	free(this->descripcion);
	free(this);
	return 0;
}
