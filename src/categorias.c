#include "categorias.h"
#include <string.h>
#include "hash_extensible.h"
#include "config.h"
#include "serializador.h"
#include "autoincrement.h"

static THashExtensible* hash_categorias = NULL;
static unsigned int categorias_last_id = 0;

struct TCategoria {
	unsigned int id;
	char* nombre;
	char* descripcion;
};

unsigned int Categorias_get_id(uint8_t* ele, size_t size){
	unsigned int id = * ( (unsigned int*) ele);
	return id;
}

static TCategoria* _categoriaDesdeBuf(uint8_t* buf, size_t size);

static uint8_t* _catBufDesdeData(
	unsigned int id,
	char *nombre,
	char *descripcion,
	size_t *size_out
);

int Categorias_init(){
	hash_categorias = HashExtensible_crear(
		CATEGORIAS_HASH_BLOQUE_PATH,
		CATEGORIAS_HASH_TABLA_PATH,
		CATEGORIAS_HASH_BAJA_PATH,
		CATEGORIAS_HASH_BLOQUE_SIZE,
		&HashDispersionModulo,
		&Categorias_get_id
	);

	if(!hash_categorias)
		return 1;

	categorias_last_id = getLastId(CATEGORIAS_LAST_ID_FILE);

	return 0;
}

int Categorias_end(){
	HashExtensible_destruir(hash_categorias);
	writeLastId(CATEGORIAS_LAST_ID_FILE, categorias_last_id);
	hash_categorias = NULL;
	categorias_last_id = 0;
	return 0;
}

TCategoria* Categoria_new(
	char* nombre,
	char* desc
){
	unsigned int last_id = categorias_last_id;
	size_t size = 0;
	uint8_t* buf = _catBufDesdeData(
		last_id+1,
		nombre,
		desc,
		&size
	);

	if(HashExtensible_insertar(hash_categorias, buf, size)){
		free(buf);
		return NULL;
	}

	TCategoria* cat = _categoriaDesdeBuf(buf, size);
	free(buf);

	categorias_last_id++;
	return cat;
}

static uint8_t* _catBufDesdeData(
	unsigned int id,
	char *nombre,
	char *descripcion,
	size_t *size_out
){
	size_t size = 0;
	SerializadorData data = {0};

	uint8_t *buf = Serializador_pack(NULL, SER_INT, (SerializadorData*) id, &size);

	data.buf = (uint8_t*) nombre;
	data.size = strlen(nombre) + 1;
	buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);

	char aux[1] = {0};
	if(!descripcion)
		descripcion = aux;

	// Serializo descripcion
	data.buf = (uint8_t*) descripcion;
	data.size = strlen(descripcion) + 1;
	buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);

	*size_out = size;
	return buf;
}

static TCategoria* _categoriaDesdeBuf(uint8_t* buf, size_t buf_size){
	TCategoria* this = (TCategoria*) calloc(1, sizeof(TCategoria));
	SerializadorData data;
	size_t size = 0;
	size_t pos = 0;
	data.buf = buf;
	data.size = buf_size;

	//id
	this->id = * ( (unsigned int*) Serializador_unpack(&data, SER_INT, &size, &pos));

	//Nombre
	char * str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
	this->nombre = strcpy(malloc(size), str_aux);

	//descripcion
	str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
	this->descripcion = strcpy(malloc(size), str_aux);

	return this;
}

int Categoria_free(TCategoria* this){
	if(!this)
		return 1;

	free(this->nombre);
	free(this->descripcion);
	free(this);
	return 0;
}

TCategoria* Categoria_from_id(unsigned int id){
	uint8_t* buf;
	size_t size;

	if( ! (buf = HashExtensible_get(hash_categorias, id, &size))){
		return NULL;
	}

	TCategoria* cat = _categoriaDesdeBuf(buf, size);
	free(buf);

	return cat;
}

TCategoria* Categoria_all(unsigned int *id_p){
	unsigned int last_id = categorias_last_id;
	if(*id_p >= last_id)
		return NULL;

	TCategoria* cat = NULL;
	do {
		free(cat);
		cat = Categoria_from_id(++(*id_p));
	}while(*id_p < last_id && ! cat);

	return cat;
}

unsigned int Categoria_get_id(TCategoria* this){
	if(!this)
		return 0;
	return this->id;
}
const char* Categoria_get_nombre(TCategoria* this){
	if(!this)
		return NULL;
	return this->nombre;
}
const char* Categoria_get_descripcion(TCategoria* this){
	if(!this)
		return NULL;
	return this->descripcion;
}
